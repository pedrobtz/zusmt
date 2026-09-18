#!/bin/sh
# Re-create src/opensmt/ from upstream OpenSMT at the pinned version.
#
# Maintainer-side only: this needs git, bison >= 3.0 and flex >= 2.6, none of
# which may be required to *install* the package. That is why the generated
# parser is committed rather than built at install time.
#
# Idempotent by construction: it deletes src/opensmt/ and rebuilds it, so
# re-running on a clean checkout must leave `git status` clean. If it does
# not, the vendored tree has been hand-edited -- which is exactly what
# tools/vendor/verify and the vendor.yml workflow exist to catch.
#
# To bump: change OPENSMT_VERSION, re-run, review the diff, run the test
# suite. The patches under tools/patches/ are applied last and are expected
# to need attention when they stop applying -- that is the signal that
# upstream changed something this package depends on.
set -eu

OPENSMT_VERSION=v2.9.2
OPENSMT_REPO=https://github.com/usi-verification-and-security/opensmt.git

# Directories dropped from the import, with the reason:
#   parallel/  needs sockets and threads-with-signals; the R package solves
#              in-process and CRAN would not thank us for either
#   bin/       the opensmt command-line executable; a package builds a
#              shared object, not a program
PRUNE="parallel bin"

BISON="${BISON:-bison}"
FLEX="${FLEX:-flex}"

here=$(cd "$(dirname "$0")/.." && pwd)
vendor="${here}/src/opensmt"
meta="${here}/tools/vendor"

for tool in git "${BISON}" "${FLEX}"; do
  command -v "${tool}" >/dev/null 2>&1 || {
    echo "ERROR: ${tool} not found; it is required to re-vendor." >&2
    exit 1
  }
done

case $("${BISON}" --version | head -1) in
  *' 1.'*|*' 2.'*)
    echo "ERROR: bison >= 3.0 required (upstream asks for it); found:" >&2
    "${BISON}" --version | head -1 >&2
    echo "On macOS the system bison is 2.3 -- try BISON=/usr/local/opt/bison/bin/bison" >&2
    exit 1
    ;;
esac

tmp=$(mktemp -d)
trap 'rm -rf "${tmp}"' EXIT

echo "==> cloning ${OPENSMT_REPO} at ${OPENSMT_VERSION}"
git clone --quiet --depth 1 --branch "${OPENSMT_VERSION}" "${OPENSMT_REPO}" "${tmp}/osmt"
sha=$(git -C "${tmp}/osmt" rev-parse HEAD)
date=$(git -C "${tmp}/osmt" log -1 --format=%cI)

echo "==> importing src/ -> src/opensmt/"
rm -rf "${vendor}"
mkdir -p "${vendor}"
(cd "${tmp}/osmt/src" && tar cf - .) | (cd "${vendor}" && tar xf -)

for d in ${PRUNE}; do
  echo "==> pruning ${d}/"
  rm -rf "${vendor:?}/${d}"
done

# The CMakeLists.txt files are KEPT, despite this package not using CMake.
# They are the only authoritative statement of which sources upstream
# actually compiles, and that is not the same as "every .cc in the tree":
# v2.9.2 ships bvsolver/ and logics/BVLogic.cc but comments both out of the
# build, and they do not compile (BVLogic is left an incomplete type).
# tools/objects.sh reads these files to build the object list.

# The SMT-LIB parser. Upstream generates these at build time; a CRAN package
# cannot, so they are generated here and committed.
echo "==> generating the SMT-LIB parser (bison $(${BISON} --version | head -1 | sed 's/.* //'), $(${FLEX} --version))"
(
  cd "${vendor}/parsers/smt2new"
  "${BISON}" --defines=smt2newparser.hh -o smt2newparser.cc smt2newparser.yy
  "${FLEX}" -o smt2newlexer.cc smt2newlexer.ll
)

# Everything upstream does that R forbids -- console writes, the system RNG,
# process termination -- is rewritten here, before the checksums are taken, so
# the recorded tree is the patched one and verify checks what is compiled.
echo "==> applying R-hosting patches"
"${here}/tools/patches.sh"

# Provenance and checksums. checksums.sha256 is what tools/vendor/verify
# checks and what the vendor.yml workflow requires a PR to update alongside
# any change under src/opensmt/.
echo "==> writing provenance and checksums"
mkdir -p "${meta}"
{
  printf 'upstream\t%s\n' "${OPENSMT_REPO}"
  printf 'version\t%s\n' "${OPENSMT_VERSION}"
  printf 'commit\t%s\n' "${sha}"
  printf 'committed\t%s\n' "${date}"
  printf 'pruned\t%s\n' "${PRUNE}"
  printf 'patched\ttools/patches.sh (console output, RNG, process exit)\n'
  printf 'generated\tparsers/smt2new/smt2newparser.cc, smt2newparser.hh, smt2newlexer.cc\n'
  printf 'bison\t%s\n' "$("${BISON}" --version | head -1)"
  printf 'flex\t%s\n' "$("${FLEX}" --version)"
  printf 'files\t%s\n' "$(find "${vendor}" -type f ! -name '*.o' ! -name '*.so' ! -name '*.dll' ! -name '*.dylib' ! -name '*.a' | wc -l | tr -d ' ')"
} > "${meta}/manifest.tsv"

# Build artifacts are excluded: R compiles in place, so a build leaves .o
# files inside src/opensmt/, and recording them would make the checksums
# depend on whether the tree had been built.
(cd "${here}" && find src/opensmt -type f ! -name '*.o' ! -name '*.so' ! -name '*.dll' ! -name '*.dylib' ! -name '*.a' | LC_ALL=C sort | xargs shasum -a 256) \
  > "${meta}/checksums.sha256"

# The build has to follow the tree: a bump that adds or drops a source file
# must update src/Makevars.in, or the new file silently never gets compiled.
echo "==> regenerating the object list"
"${here}/tools/objects.sh"

echo "==> vendored $(find "${vendor}" -type f ! -name '*.o' ! -name '*.so' ! -name '*.dll' ! -name '*.dylib' ! -name '*.a' | wc -l | tr -d ' ') files from ${OPENSMT_VERSION} (${sha})"
