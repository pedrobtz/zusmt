# Shared configure logic for zusmt, sourced by ./configure and ./configure.win.
#
# Stage 1 of roadmap.md: prove that R's own toolchain can build the three
# things OpenSMT needs -- a C++20 compiler, GMP (C and C++ bindings) and
# threads -- before vendoring ~90 translation units that depend on all three.
#
# Sets, for substitution into src/Makevars.in:
#   GMP_CFLAGS  GMP_LIBS  PTHREAD
#
# Every probe is logged. The point of this script in CI is not only to
# succeed or fail but to say, in the check log, *how* GMP was found on each
# platform -- that is the Stage 1 finding.

: "${R_HOME:=$(R RHOME)}"
RBIN="${R_HOME}/bin/R"

CXX=$("${RBIN}" CMD config CXX20)
CXXSTD=$("${RBIN}" CMD config CXX20STD)
CXXFLAGS=$("${RBIN}" CMD config CXX20FLAGS)
CPPFLAGS=$("${RBIN}" CMD config CPPFLAGS)
LDFLAGS=$("${RBIN}" CMD config LDFLAGS)

echo "*** zusmt configure"
echo "**  R          : $("${RBIN}" --version | head -1)"
echo "**  CXX20      : ${CXX} ${CXXSTD}"

if [ -z "${CXX}" ] || [ -z "${CXXSTD}" ]; then
  cat >&2 <<'MSG'
ERROR: this R has no C++20 compiler configured.

  `R CMD config CXX20` (or CXX20STD) came back empty, which means R was
  configured without a C++20 toolchain. zusmt bundles OpenSMT, which sets
  CMAKE_CXX_STANDARD 20 and does not build under C++17.

  Install a C++20-capable compiler (GCC >= 10, clang >= 10) and reconfigure R,
  or set CXX20/CXX20STD in ~/.R/Makevars.
MSG
  exit 1
fi

# One probe covering all three requirements at once: mpq_class is GMP's C++
# binding (so it needs both -lgmpxx and -lgmp), the lambda runs on a
# std::thread, and the static_assert pins the language standard.
write_probe() {
  cat > conftest.cc <<'EOT'
#include <gmpxx.h>
#include <thread>
#include <string>

static_assert(__cplusplus >= 202002L, "C++20 required");

template <typename T>
concept Addable = requires(T a, T b) { a + b; };

template <Addable T>
static T add(T a, T b) { return a + b; }

int main() {
  mpq_class a(1, 3), b(1, 6), c;
  std::string s;
  std::thread t([&] { c = add(a, b); c.canonicalize(); s = c.get_str(); });
  t.join();
  return s == "1/2" ? 0 : 1;
}
EOT
}

# $1 = extra cppflags, $2 = libs, $3 = pthread flag
try_build() {
  write_probe
  # shellcheck disable=SC2086
  ${CXX} ${CXXSTD} ${CXXFLAGS} ${CPPFLAGS} $1 $3 conftest.cc ${LDFLAGS} $2 $3 \
    -o conftest >conftest.log 2>&1
}

# GMP's headers must be included as *system* headers, not with -I.
#
# gmpxx.h 6.3.0 declares its literal operators as `operator "" _mpz`, with a
# space, which clang 18+ deprecates (-Wdeprecated-literal-operator). R CMD
# check promotes compiler output during install to "significant warnings" and
# a WARNING overall, so on macOS -- where Homebrew's prefix is a *user*
# include path, unlike /usr/include on Linux -- three warnings from a header
# this package does not own failed the whole check leg. -isystem suppresses
# diagnostics from that directory, which is what it is for. It matters more
# once ~90 vendored OpenSMT files include gmpxx.h.
to_isystem() {
  _out=""
  for _tok in $1; do
    case "${_tok}" in
      -I?*) _out="${_out} -isystem ${_tok#-I}" ;;
      *)    _out="${_out} ${_tok}" ;;
    esac
  done
  printf '%s' "${_out# }"
}

probe() {
  _cflags="$1"
  _libs="$2"
  _alt=$(to_isystem "${_cflags}")

  # Prefer the -isystem form; fall back to -I for a compiler that rejects it.
  if [ "${_alt}" != "${_cflags}" ]; then
    _forms="isystem plain"
  else
    _forms="plain"
  fi

  for _form in ${_forms}; do
    if [ "${_form}" = "isystem" ]; then
      _cf="${_alt}"
    else
      _cf="${_cflags}"
    fi
    for _pt in "" "-pthread"; do
      if try_build "${_cf}" "${_libs}" "${_pt}"; then
        GMP_CFLAGS="${_cf}"
        GMP_LIBS="${_libs}"
        PTHREAD="${_pt}"
        echo "**  GMP found  : cflags='${GMP_CFLAGS}' libs='${GMP_LIBS}' pthread='${PTHREAD}'"
        return 0
      fi
    done
  done
  echo "**  no       : cflags='${_cflags}' libs='${_libs}'"
  return 1
}

find_gmp() {
  # 1. Caller override, honoured before anything is guessed.
  if [ -n "${GMP_CFLAGS:-}${GMP_LIBS:-}" ]; then
    echo "**  trying    : GMP_CFLAGS/GMP_LIBS from the environment"
    probe "${GMP_CFLAGS:-}" "${GMP_LIBS:--lgmpxx -lgmp}" && return 0
  fi

  # 2. pkg-config. gmp ships a .pc on most distributions; gmpxx does not
  #    always, so take gmp's cflags and add the C++ library by hand.
  if command -v pkg-config >/dev/null 2>&1; then
    for _mod in gmpxx gmp; do
      if pkg-config --exists "${_mod}" 2>/dev/null; then
        echo "**  trying    : pkg-config ${_mod} ($(pkg-config --modversion "${_mod}"))"
        _pc_cflags=$(pkg-config --cflags "${_mod}")
        _pc_libs=$(pkg-config --libs "${_mod}")
        case "${_pc_libs}" in
          *-lgmpxx*) ;;
          *) _pc_libs="-lgmpxx ${_pc_libs}" ;;
        esac
        probe "${_pc_cflags}" "${_pc_libs}" && return 0
      fi
    done
  fi

  # 3. The compiler's own search path. This is the Windows/Rtools case and
  #    the Linux-with-libgmp-dev case.
  echo "**  trying    : compiler default search path"
  probe "" "-lgmpxx -lgmp" && return 0

  # 4. Common install prefixes, Homebrew first on macOS.
  for _p in "$(brew --prefix gmp 2>/dev/null)" /opt/homebrew /usr/local /opt/local /usr; do
    [ -n "${_p}" ] || continue
    [ -f "${_p}/include/gmpxx.h" ] || continue
    echo "**  trying    : prefix ${_p}"
    probe "-I${_p}/include" "-L${_p}/lib -lgmpxx -lgmp" && return 0
  done

  return 1
}

if ! find_gmp; then
  echo "--- last probe log ---" >&2
  [ -f conftest.log ] && cat conftest.log >&2
  cat >&2 <<'MSG'

ERROR: could not build against GMP (the GNU Multiple Precision library).

  zusmt bundles OpenSMT, which requires GMP and its C++ bindings (gmpxx).
  Install the development package and retry:

    Debian/Ubuntu   apt-get install libgmp-dev
    Fedora/RHEL     dnf install gmp-devel
    macOS           brew install gmp
    Windows         provided by Rtools

  If GMP lives somewhere unusual, point the build at it:

    R CMD INSTALL --configure-vars='GMP_CFLAGS=-I/path/include GMP_LIBS="-L/path/lib -lgmpxx -lgmp"' .
MSG
  rm -rf conftest conftest.cc conftest.log conftest.exe conftest.dSYM
  exit 1
fi

rm -rf conftest conftest.cc conftest.log conftest.exe conftest.dSYM
