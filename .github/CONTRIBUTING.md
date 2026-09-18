# Contributing to zusmt

## The bundled solver

`src/opensmt/` is **generated**. Do not edit anything under it: `tools/vendor/verify` and the
`vendor` workflow both check it against `tools/vendor/checksums.sha256`, and a hand edit is lost at
the next version bump anyway.

To change the vendored sources, change the script that produces them.

### Bumping OpenSMT

```sh
# 1. Edit OPENSMT_VERSION in tools/vendor.sh
# 2. Re-import. macOS ships bison 2.3; upstream needs >= 3.0.
BISON=/usr/local/opt/bison/bin/bison ./tools/vendor.sh

# 3. Check Windows before pushing. This is where it breaks.
./tools/check-mingw.sh            # compile errors
./tools/check-mingw.sh --link     # duplicate definitions

# 4. Build and test
R CMD INSTALL --preclean .
Rscript -e 'devtools::test()'
```

`tools/vendor.sh` deletes and rebuilds the tree, then applies `tools/patches.sh`, regenerates the
object list and records checksums. Re-running it on a clean checkout must leave `git status` clean.

### Why the patches exist

`tools/patches.sh` rewrites the upstream sources so they can live inside R. Each rule asserts that it
changed something, so a bump that moves a call site fails loudly rather than silently leaving one
behind. The rules fall into three groups:

- **R hosting** — console output through `Rprintf` rather than `stdout`, a self-contained RNG
  instead of `rand()`, exceptions instead of `exit()`. `R CMD check` inspects the shared object for
  these symbols and CRAN rejects them.
- **Portability** — Windows has no `<sys/resource.h>`, `long` is 32 bits there, and `asprintf` and
  `dprintf` do not exist. Porting v2.9.2 took six CI round trips before `tools/check-mingw.sh`
  existed; use it.
- **Interruptibility** — the solver's own `okContinue()` also asks whether R has a pending
  interrupt, so a long search can be stopped without a `longjmp` crossing C++ frames.

## Tests

```sh
Rscript -e 'devtools::test()'
Rscript -e 'devtools::check()'
```

Adding a regression case is one file in `inst/smt2/` with two header comments — `; logic:` and
`; expect:` — and no edit to any test.

The supported logics are defined once, in `kSupportedLogics[]` in `src/solver.cc`. `smt_logics()`
reads that array and the tests and documentation derive from it, so adding a logic without a
satisfiable and an unsatisfiable probe in `tests/testthat/test-logics.R` fails the suite.

## CI

`R-CMD-check` runs on the GitHub runners and in CRAN-like containers; `native-checks` runs valgrind,
gctorture, rchk and LTO; `vendor` guards the bundled sources; `coverage` reports both R and native
coverage. The `sanitizers` leg is parked pending a fix in
[r-actions](https://github.com/pedrobtz/r-actions) — see the comment in
`.github/workflows/native-checks.yml`.
