#!/bin/sh
# Syntax-check the vendored tree against Windows headers, locally.
#
# Windows is where this package breaks: mingw is LLP64 (long is 32 bits, unlike
# every other platform here) and lacks POSIX headers and functions upstream
# assumes. Finding those one at a time through CI costs a five-minute round
# trip each, and porting v2.9.2 took five of them -- a missing header, a
# missing typedef, an LLP64 cast, then dprintf/asprintf together.
#
# This compiles every source the package builds with the mingw cross-compiler,
# using the real Windows headers and a 32-bit long, and reports every failure
# at once. Run it after a version bump, before pushing.
#
#   brew install mingw-w64      # or the distribution equivalent
#   ./tools/check-mingw.sh
#
# It is a syntax check, not a build: R's and GMP's headers come from this
# machine, so it proves nothing about linking. It catches the class of problem
# that has actually bitten.
set -eu

here=$(cd "$(dirname "$0")/.." && pwd)
CXX="${MINGW_CXX:-x86_64-w64-mingw32-g++}"

command -v "${CXX}" >/dev/null 2>&1 || {
  echo "ERROR: ${CXX} not found. Install mingw-w64, or set MINGW_CXX." >&2
  exit 1
}

R_INC="$(R RHOME)/include"
GMP_INC="${GMP_INCLUDE_DIR:-}"
if [ -z "${GMP_INC}" ]; then
  for candidate in "$(brew --prefix gmp 2>/dev/null)/include" /usr/local/include /usr/include; do
    [ -f "${candidate}/gmpxx.h" ] && { GMP_INC="${candidate}"; break; }
  done
fi
[ -n "${GMP_INC}" ] || { echo "ERROR: gmpxx.h not found; set GMP_INCLUDE_DIR." >&2; exit 1; }

cd "${here}/src"
sources=$(sed -n '/^OPENSMT_OBJECTS/,/^# <<</p' Makevars.in |
  grep -oE 'opensmt/[A-Za-z0-9_/.+-]+\.o' | sed 's/\.o$/.cc/')

echo "==> $(printf '%s\n' "${sources}" | wc -l | tr -d ' ') sources against $(${CXX} --version | head -1)"

failed=0
for f in ${sources}; do
  if ! "${CXX}" -std=gnu++20 -fsyntax-only -DNDEBUG \
       -I. -Iopensmt -I"${R_INC}" -I"${GMP_INC}" "${f}" > /tmp/zusmt-mingw.$$ 2>&1; then
    failed=$((failed + 1))
    echo "--- ${f}"
    grep -E 'error:' /tmp/zusmt-mingw.$$ | head -3
  fi
  rm -f /tmp/zusmt-mingw.$$
done

if [ "${failed}" -ne 0 ]; then
  echo "==> ${failed} source(s) would not compile on Windows" >&2
  exit 1
fi
echo "==> all sources compile under mingw (long is 32-bit there)"
