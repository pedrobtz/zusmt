#!/bin/sh
# Rewrite the vendored OpenSMT tree so it can live inside R.
#
# `R CMD check` inspects the shared object for symbols that a package must not
# have: writes to stdout/stderr, the system RNG, and calls that terminate the
# process. Upstream is a command-line solver and uses all three, so the choice
# is to patch it or to fail the check -- and since these have to be re-applied
# at every version bump, they are rules in a script rather than edits in a
# diff. Each rule asserts that it changed something: if a bump makes one a
# no-op, this fails rather than silently leaving the call site behind.
#
# Run by tools/vendor.sh, after the import and the parser generation.
set -eu

here=$(cd "$(dirname "$0")/.." && pwd)
vendor="${here}/src/opensmt"

[ -d "${vendor}" ] || { echo "ERROR: ${vendor} missing; run tools/vendor.sh" >&2; exit 1; }

# sed -i is spelled differently on BSD and GNU; do it by hand instead.
edit() { # file, sed-expression
  _f="$1"
  _e="$2"
  sed "${_e}" "${_f}" > "${_f}.patched"
  mv "${_f}.patched" "${_f}"
}

# Apply an expression across every file matching a grep pattern, and report
# how many files changed. rule <name> <grep-ere> <sed-ere>
rule() {
  _name="$1"
  _match="$2"
  _expr="$3"
  _n=0
  for f in $(grep -rlE "${_match}" "${vendor}" --include='*.cc' --include='*.h' 2>/dev/null || true); do
    before=$(shasum -a 256 "${f}" | cut -d' ' -f1)
    edit "${f}" "${_expr}"
    after=$(shasum -a 256 "${f}" | cut -d' ' -f1)
    [ "${before}" = "${after}" ] || _n=$((_n + 1))
  done
  if [ "${_n}" -eq 0 ]; then
    echo "ERROR: rule '${_name}' changed nothing -- upstream moved, re-derive it" >&2
    exit 1
  fi
  echo "    ${_name}: ${_n} files"
}

echo "==> patching the vendored tree for R"

# 1. Console output. std::cout/std::cerr become ostreams writing through
#    Rprintf/REprintf, so '<<' chains keep working untouched.
rule 'std::cout -> zusmt::rout()' 'std::cout' 's/std::cout/zusmt::rout()/g'
rule 'std::cerr -> zusmt::rerr()' 'std::cerr' 's/std::cerr/zusmt::rerr()/g'

# 2. C-style output. The leading character class avoids snprintf/vsnprintf,
#    whose names end in printf; there is no std::printf in this tree.
rule 'printf -> Rprintf' '(^|[^_[:alnum:]])printf[[:space:]]*\(' \
  's/\([^_[:alnum:]]\)printf[[:space:]]*(/\1Rprintf(/g; s/^printf[[:space:]]*(/Rprintf(/g'
# fprintf(stderr, ...) only. The remaining fprintf calls write to a real FILE*
# (proof output), and rewriting those would send a file to the console.
rule 'fprintf(stderr -> REprintf' 'fprintf[[:space:]]*\([[:space:]]*stderr[[:space:]]*,' \
  's/fprintf[[:space:]]*([[:space:]]*stderr[[:space:]]*,[[:space:]]*/REprintf(/g'

# 3. The system RNG. Both call sites are heuristic tie-breaks in interpolation
#    and proof transformation, not anything a user seeds.
rule 'rand -> zusmt::pseudo_rand' '(^|[^_[:alnum:]])rand[[:space:]]*\(\)' \
  's/\([^_[:alnum:]]\)rand[[:space:]]*()/\1zusmt::pseudo_rand()/g'
rule 'srand -> zusmt::pseudo_srand' '(^|[^_[:alnum:]])srand[[:space:]]*\(' \
  's/\([^_[:alnum:]]\)srand[[:space:]]*(/\1zusmt::pseudo_srand(/g; s/^srand[[:space:]]*(/zusmt::pseudo_srand(/g'

# 4. Process termination. These are targeted rather than a blanket rule
#    because Interpret has a *method* named exit(), and rewriting that would
#    not compile.
echo "    exit()/abort() -> zusmt::fatal(), per call site:"
patch_exact() { # file, sed-expression, description
  _f="${vendor}/$1"
  [ -f "${_f}" ] || { echo "ERROR: $1 not found" >&2; exit 1; }
  before=$(shasum -a 256 "${_f}" | cut -d' ' -f1)
  edit "${_f}" "$2"
  after=$(shasum -a 256 "${_f}" | cut -d' ' -f1)
  [ "${before}" != "${after}" ] || { echo "ERROR: no-op patch: $1 ($3)" >&2; exit 1; }
  echo "      $1: $3"
}
patch_exact tsolvers/lasolver/Simplex.cc \
  's/^[[:space:]]*exit(1);/    zusmt::fatal("bundled solver: inconsistent bound in Simplex::overBound");/' \
  'exit(1) after an assertion'
patch_exact common/numbers/FastRational.h \
  's/if (value < 1) abort()/if (value < 1) zusmt::fatal("bundled solver: non-positive value where positive required")/' \
  'abort() in CHECK_POSITIVE'
patch_exact api/DimacsParser.h \
  's/exit(3);/zusmt::fatal("bundled solver: malformed DIMACS input");/g' \
  'exit(3) on malformed input'

# The generated lexer terminates on a syntax error and on its own internal
# fatal path. Patched here because vendor.sh regenerates it, so an edit to
# the .ll upstream would not survive.
patch_exact parsers/smt2new/smt2newlexer.cc \
  's/exit( YY_EXIT_FAILURE );/zusmt::fatal("bundled solver: lexer fatal error");/; s/exit(1); }/zusmt::fatal("SMT-LIB syntax error"); }/; s/exit( 1 ); }/zusmt::fatal("SMT-LIB syntax error"); }/' \
  'exit() in the flex skeleton and error rules'

#    The parser reports syntax errors through Rprintf, which goes straight to
#    the console. The R API captures the solver's output so it can raise a
#    condition instead (zusmt::begin_capture), and that only sees the ostream
#    shims -- so a syntax error printed this way leaked to the console
#    alongside the R error raised for it. Routing it through zusmt::rerr()
#    puts it where the capture can find it.
patch_exact parsers/smt2new/smt2newparser.cc \
  's|Rprintf("At interactive input: %s\\n", s);|zusmt::rerr() << "At interactive input: " << s << "\\n";|; s|Rprintf( "At line %d: %s\\n", locp->first_line, s );|zusmt::rerr() << "At line " << locp->first_line << ": " << s << "\\n";|' \
  'parser errors via rerr(), so the R API can capture them'

# 5. The stdout/stderr FILE* symbols themselves. R CMD check looks for these
#    in the shared object, so rewriting the calls that use them is not enough;
#    the remaining references have to go too.
echo "    stdout/stderr FILE* references:"
#    The same line is also the tree's only GNU named-variadic macro, and
#    clang -pedantic reports `args...` and `, ## args` as significant
#    warnings, which fails the check on CRAN's clang container. __VA_OPT__ is
#    standard C++20 and this package already compiles as C++20.
patch_exact common/ReportUtils.h \
  's/#define reportf(format, args...) ( fflush(stdout), REprintf(format, ## args), fflush(stderr) )/#define reportf(format, ...) ( REprintf(format __VA_OPT__(,) __VA_ARGS__) )/' \
  'reportf: drop fflush, and use __VA_OPT__ rather than the GNU comma-paste'
patch_exact smtsolvers/CoreSMTSolver.cc \
  's|^\([[:space:]]*\)fflush(stderr);|\1/* fflush(stderr) dropped: output goes through REprintf */|' \
  'fflush(stderr) after a progress line'
#    flex initialises the reentrant scanner to stdin/stdout. Never read:
#    Smt2newContext::init_scanner() always calls yyset_in() or
#    yy_scan_string() and returns -1 when given neither, and the .ll has a
#    catch-all rule, so ECHO (the only user of yyout) is unreachable.
patch_exact parsers/smt2new/smt2newlexer.cc \
  's/yyin = stdin;/yyin = NULL;  \/* zusmt: set by yyset_in(); never read *\//; s/yyout = stdout;/yyout = NULL;  \/* zusmt: ECHO is unreachable *\//' \
  'yyin = stdin / yyout = stdout defaults'

#    A class-scope `thread_local` of non-trivial type does not link on mingw.
#    mpz_class needs dynamic initialisation, so the compiler emits a TLS init
#    wrapper; on ELF that wrapper is COMDAT and the linker folds the copies,
#    but GCC targeting PE-COFF emits it without COMDAT linkage, so all 28
#    translation units that touch FastRational contribute a definition and ld
#    rejects every one after the first. The member on the line above, `pool`,
#    proves it is the thread_local and not `inline static`: same storage
#    class, same 28 users, links fine.
#    Moving it inside the accessor keeps the per-thread semantics upstream
#    asked for while confining access to the function body, so no
#    externally-visible wrapper is emitted. `temp` is named in exactly one
#    other place -- the accessor itself -- so this reaches every use.
patch_exact common/numbers/FastRational.h \
  '/^    inline static thread_local mpz_class temp;$/d; s|inline static mpz_ptr mpz() { return temp.get_mpz_t(); }|inline static mpz_ptr mpz() { static thread_local mpz_class temp; return temp.get_mpz_t(); }|' \
  'TLS: move thread_local into the accessor, so mingw emits no TLS wrapper'

# 6. POSIX headers that Windows does not have. Upstream targets Linux and
#    macOS; mingw has neither <sys/resource.h> nor <sys/wait.h>, and the
#    Windows build fails at the first file that includes Timer.h.
echo "    Windows portability:"
patch_exact common/Timer.h \
  's|#include <sys/resource.h>|#include <r_rusage.h>|' \
  'sys/resource.h -> the getrusage() shim'
patch_exact common/SystemQueries.h \
  's|#include <sys/resource.h>|#include <r_rusage.h>|' \
  'sys/resource.h -> the getrusage() shim'
#    Both <sys/wait.h> includes are vestigial: neither file calls fork(),
#    wait(), waitpid() or anything else from it.
patch_exact tsolvers/THandler.cc \
  '/#include <sys\/wait.h>/d' \
  'drop unused sys/wait.h'
patch_exact tsolvers/egraph/UFInterpolator.cc \
  '/#include <sys\/wait.h>/d' \
  'drop unused sys/wait.h'

# 7. The one libgmpxx C++ symbol. gmpxx's operator<<(ostream&, mpq) is the
#    only thing this package used from the *library* rather than the header,
#    and it is compiled against whichever C++ standard library built libgmpxx.
#    On a libc++ toolchain linking a libstdc++-built libgmpxx (CRAN's clang23
#    container, FreeBSD, mixed setups) it does not resolve and the .so fails
#    to load with an undefined symbol. get_str() is header-only -- it calls
#    the C __gmpq_get_str -- and prints identically in base 10, so this drops
#    the ABI dependency instead of working around it.
echo "    libgmpxx ABI:"
patch_exact common/numbers/FastRational.cc \
  's|out << (sign?"(- ":"") << mpq_c << (sign?")":"");|out << (sign?"(- ":"") << mpq_c.get_str() << (sign?")":"");|; s|^        out << mpq;|        out << mpq_class(mpq).get_str();|' \
  'stream mpq via get_str() rather than gmpxx operator<<'

# 8. Windows is LLP64: long is 32 bits there, 64 on Linux and macOS.
#    getMpq() asserts sizeof(long) == 8 and then casts both parts of the
#    word-sized representation through long. The assert is not paranoia --
#    `den` is a uint32_t, and on a 32-bit signed long a denominator above
#    INT32_MAX converts to a negative value, which would silently produce a
#    wrong rational rather than fail. But num/den are int32_t/uint32_t, so
#    they fit exactly once each goes through the matching signedness, and the
#    two-argument mpq_class constructor takes mpz_class -- which has a ctor
#    for unsigned long. So: fix the hazard the assert was guarding, rather
#    than deleting the guard and hoping.
echo "    LLP64 (Windows long is 32-bit):"
patch_exact common/numbers/FastRational.h \
  '/static_assert(sizeof(long) == 8);/d; s|return mpq_class{static_cast<long>(num), static_cast<long>(den)};|return mpq_class{static_cast<long>(num), static_cast<unsigned long>(den)};|' \
  'getMpq(): denominator through unsigned long, not long'
#    The same width difference one file over, without a correctness hazard
#    behind it: PolynomialT::size() is declared std::size_t and defined
#    unsigned long -- one type on LP64, two on LLP64, where the definition
#    then matches no declaration. poly.size() already returns std::size_t and
#    was being narrowed on the way out.
patch_exact common/polynomials/Polynomial.h \
  's/^unsigned long PolynomialT<VarType>::size() const {/std::size_t PolynomialT<VarType>::size() const {/' \
  'size(): definition must say std::size_t, not unsigned long'

#    Under LTO, GCC's value-range propagation cannot see that `newsize` only
#    ever holds an entry of the positive `primes[]` table, concludes it could
#    be negative, and reports the resulting conversion as an allocation of
#    18446744073709551615 bytes -- (size_t)-1 -- which R CMD check promotes to
#    a WARNING. CRAN runs an LTO flavour, so this arrives in a submission
#    report otherwise.
#    A cast would silence the diagnostic while leaving the same conversion;
#    the guard makes the range provable instead. It is unreachable in fact,
#    which is the point: it tells the compiler what the table already
#    guarantees.
patch_exact minisat/mtl/Map.h \
  's|        table = new vec<Pair>\[newsize\];|        if (newsize <= 0) newsize = primes[0];  /* zusmt: bound for -Walloc-size-larger-than under LTO */\n        table = new vec<Pair>[newsize];|; s|        table = new std::vector<Pair>\[newsize\];|        if (newsize <= 0) newsize = primes[0];  /* zusmt: bound for -Walloc-size-larger-than under LTO */\n        table = new std::vector<Pair>[newsize];|' \
  'Map.h: bound newsize so LTO value-range analysis can see it is positive'

# 9. The only rule here that changes what the solver DOES, rather than what it
#    can compile against or where it writes. Everything above keeps upstream's
#    behaviour and makes it portable or R-safe; this one gives the search a
#    reason to stop that upstream never had.
echo "    interruptibility:"
#    okContinue() is the solver's own
#    termination check, called from the innermost loop, and it already ends
#    the search by returning false -- unwinding normally through every
#    destructor. Asking about a pending R interrupt in the same place gets
#    interruptibility with upstream's own control flow, rather than a longjmp
#    across C++ frames, which would skip those destructors and leak the
#    solver's heap. check() then returns s_Undef, and the boundary raises the
#    interrupt in R once no C++ frame is left.
patch_exact smtsolvers/CoreSMTSolver.cc \
  's|^    return not opensmt::stop;|    return not opensmt::stop and not zusmt::interrupt_requested();|' \
  'okContinue(): also stop when R has a pending interrupt'


# 10. Functions that are POSIX or GNU rather than standard, and one signature
#    that only mismatches under LLP64. All found by the Windows leg.
echo "    POSIX/GNU functions absent on Windows:"
#    dprintf() writes to a file descriptor, so it is both absent on mingw and
#    a direct write to fd 2 that R forbids. Rule 2 did not catch it: the 'd'
#    in front of printf is alphanumeric, which is exactly what stops that
#    rule from mangling snprintf.
rule 'dprintf(STDERR_FILENO -> REprintf' 'dprintf[[:space:]]*\([[:space:]]*STDERR_FILENO[[:space:]]*,' \
  's/dprintf[[:space:]]*([[:space:]]*STDERR_FILENO[[:space:]]*,[[:space:]]*/REprintf(/g'
#    asprintf() is a GNU extension (15 call sites in 8 files). The shim
#    implements it with vsnprintf and malloc, and is used on every platform so
#    the call sites behave the same everywhere rather than only being fixed
#    where they failed to compile.
#    The '_' in the negated class is load-bearing, as it is in rule 2: it
#    keeps this off NumberUtils.h's gmp_asprintf(), which is GMP's own and
#    perfectly portable. Rewriting that would break a working call.
rule 'asprintf -> zusmt::alloc_printf' '(^|[^_:[:alnum:]])asprintf[[:space:]]*\(' \
  's/\([^_:[:alnum:]]\)asprintf[[:space:]]*(/\1zusmt::alloc_printf(/g; s/^asprintf[[:space:]]*(/zusmt::alloc_printf(/g'

# 11. Make the shim visible. Prepending the include is more portable than a
#    -include compiler flag and shows up in the diff.
echo "==> adding the shim include where it is needed"
n=0
for f in $(grep -rlE 'zusmt::(rout|rerr|fatal|pseudo_rand|pseudo_srand|alloc_printf|interrupt_requested)|Rprintf|REprintf' "${vendor}" --include='*.cc' --include='*.h' 2>/dev/null || true); do
  grep -q '#include <r_compat.h>' "${f}" && continue
  printf '#include <r_compat.h>\n' > "${f}.patched"
  cat "${f}" >> "${f}.patched"
  mv "${f}.patched" "${f}"
  n=$((n + 1))
done
echo "    shim include added to ${n} files"

# A class- or namespace-scope thread_local of non-trivial type is the whole
# mingw link failure above, and a version bump could reintroduce one in a file
# nothing here patches. Cheap to assert, and it needs no linker.
#
# `static` is optional in the pattern on purpose: at namespace scope
# thread_local already implies static storage duration, so `thread_local T x;`
# and C++17's `inline thread_local T x;` emit the same non-COMDAT TLS wrapper
# on mingw without the keyword ever appearing.
#
# What separates the broken shape from the fix is `^[[:space:]]*` -- the
# function-local form sits mid-line, inside the accessor. That is a formatting
# property rather than a language one, but it is one this script controls: the
# rule above writes that accessor as a single line. Reformat it across three
# lines and this guard will fire on correct code.
#
# It is an approximation, and check-mingw.sh --link is the real check: that one
# tests the actual failure rather than a proxy for it. This exists to be fast.
echo "==> checking for class-scope thread_local (does not link on mingw)"
if grep -rnE '^[[:space:]]*(inline[[:space:]]+)?(static[[:space:]]+)?thread_local' \
     "${vendor}" --include='*.cc' --include='*.h'; then
  echo "ERROR: the lines above declare a thread_local at class or namespace scope." >&2
  echo "On mingw each translation unit emits its own TLS init wrapper and the" >&2
  echo "link fails with 'multiple definition of TLS init function'. Move it" >&2
  echo "inside an accessor: static thread_local T x; within the function body." >&2
  exit 1
fi
echo "    none"
