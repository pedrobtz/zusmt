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

# 5. The stdout/stderr FILE* symbols themselves. R CMD check looks for these
#    in the shared object, so rewriting the calls that use them is not enough;
#    the remaining references have to go too.
echo "    stdout/stderr FILE* references:"
patch_exact common/ReportUtils.h \
  's/#define reportf(format, args...) ( fflush(stdout), REprintf(format, ## args), fflush(stderr) )/#define reportf(format, args...) ( REprintf(format, ## args) )/' \
  'fflush() around reportf -- REprintf writes to the R console, which R flushes'
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

# 8. Make the shim visible. Prepending the include is more portable than a
#    -include compiler flag and shows up in the diff.
echo "==> adding the shim include where it is needed"
n=0
for f in $(grep -rlE 'zusmt::(rout|rerr|fatal|pseudo_rand|pseudo_srand)|Rprintf|REprintf' "${vendor}" --include='*.cc' --include='*.h' 2>/dev/null || true); do
  grep -q '#include <r_compat.h>' "${f}" && continue
  printf '#include <r_compat.h>\n' > "${f}.patched"
  cat "${f}" >> "${f}.patched"
  mv "${f}.patched" "${f}"
  n=$((n + 1))
done
echo "    shim include added to ${n} files"
