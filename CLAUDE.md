# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working
with code in this repository.

## Purpose

`zusmt` is an R package whose reason to exist is to **vendor OpenSMT**
(the SMT solver from usi-verification-and-security/opensmt) and expose
it to R. The R layer is a thin wrapper; nearly all of the work is in
`src/` — getting a third-party C++ codebase to build reliably under
`R CMD INSTALL` on Linux, macOS and Windows (see the CI matrix in
[R-CMD-check.yaml](https://pedrobtz.github.io/zusmt/.github/workflows/R-CMD-check.yaml)).

## Current state

Stages 1–4 of [roadmap.md](https://pedrobtz.github.io/zusmt/roadmap.md)
are done: OpenSMT v2.9.2 is bundled, compiled, patched for R hosting,
and solves from R. `smoke_solve()` (internal,
[src/smoke.cc](https://pedrobtz.github.io/zusmt/src/smoke.cc)) decides
two tiny QF_UF problems and is scaffolding — the real API is Stage 6.
[`smt_toolchain()`](https://pedrobtz.github.io/zusmt/reference/smt_toolchain.md)
likewise dates from Stage 1. Read `roadmap.md` before starting work; it
says which stage the next piece belongs to.

## Vendored sources

`src/opensmt/` is generated. Never edit it by hand:
`tools/vendor/verify` and the `vendor` workflow both exist to catch
exactly that, and a hand edit is lost at the next bump.

``` sh
# bump: edit OPENSMT_VERSION in tools/vendor.sh, then
BISON=/usr/local/opt/bison/bin/bison ./tools/vendor.sh   # macOS system bison is 2.3; upstream needs >= 3.0
./tools/vendor/verify
```

`tools/vendor.sh` runs three steps after the import, and each matters:

- `tools/patches.sh` rewrites what R forbids — console writes, the
  system RNG, `exit`/`abort` — onto the shim in
  [src/r_compat.h](https://pedrobtz.github.io/zusmt/src/r_compat.h).
  Every rule asserts it changed something, so a bump that moves a call
  site fails the re-vendor rather than silently leaving it.
- `tools/objects.sh` regenerates the object list in `src/Makevars.in`
  **from upstream’s CMakeLists.txt files**, not from `find`: v2.9.2
  ships four `.cc` files it does not build, and they do not compile.
- checksums are taken last, over the patched tree, excluding `.o` files
  — R compiles in place, so a built tree carries objects inside
  `src/opensmt/`.

**Check Windows before pushing.** mingw is LLP64 (`long` is 32 bits,
unlike every other platform in the matrix) and lacks POSIX headers and
functions upstream assumes. Porting v2.9.2 took five CI round-trips — a
missing header, a missing typedef, an LLP64 cast, then
`dprintf`/`asprintf` — before `tools/check-mingw.sh` existed to find the
whole class at once:

``` sh
brew install mingw-w64
./tools/check-mingw.sh            # compile errors: missing headers, LLP64 widths, absent functions
./tools/check-mingw.sh --link     # also links the objects: duplicate definitions, e.g. class-scope thread_local
```

`--link` is slower and exists because the compile stage cannot see link
failures: a class-scope `thread_local` of non-trivial type compiles
everywhere and links nowhere on mingw, which cost a round trip of its
own.

Re-running on a clean checkout must leave `git status` clean — that is
the reproducibility property the guard depends on. A PR that changes a
file under `src/opensmt/` without changing `tools/vendor/manifest.tsv`
and `tools/vendor/checksums.sha256` fails the `vendor` workflow.

## Commands

``` r

devtools::load_all()                    # compile src/ + load; the main dev loop
devtools::document()                    # regenerate NAMESPACE + man/ from roxygen
devtools::test()                        # run all tests
devtools::test(filter = "solver")       # run tests/testthat/test-solver.R only
testthat::test_file("tests/testthat/test-solver.R")
devtools::check()                       # full R CMD check, as CI runs it
pkgdown::build_site()                   # site is published from _pkgdown.yml
```

Compiled-code changes are only picked up by a fresh `load_all()`; when
Makevars or vendored sources change, `devtools::clean_dll()` first.
`R CMD INSTALL --preclean .` is the reliable fallback.

## Vendoring constraints to respect

- **Sources must be in-tree and self-contained.** CRAN/`R CMD check`
  builds `src/` with R’s own toolchain (`Makevars`/`Makevars.win`), not
  the upstream CMake build. Vendored code is compiled by listing its
  sources, not by invoking CMake at install time; no network access
  during install.
- **Keep vendored code unmodified where possible.** Put OpenSMT under a
  clearly separated subdirectory of `src/`, keep the upstream license
  and a record of the imported version/commit (`LICENSE.note`, and
  `src/*/README` or similar), and add it to `.Rbuildignore` only if it
  must not ship. Local patches should be few, documented, and
  reapplicable when the vendored version bumps.
- **External dependencies are the hard part.** OpenSMT needs C++17 and
  GMP; anything not bundled must be discoverable via
  `configure`/`configure.win` and declared in `SystemRequirements:` in
  DESCRIPTION. Decide bundle-vs-link before writing build glue — it
  determines the whole `src/` layout.
- **The R/C++ boundary stays narrow.** Solver state lives behind
  external pointers with finalizers; upstream exceptions must never
  cross into R unhandled, and upstream must not call `exit()`/`abort()`
  or write to stdout on a check run. Registration goes through
  `@useDynLib zusmt, .registration = TRUE` (already set in
  [R/zusmt-package.R](https://pedrobtz.github.io/zusmt/R/zusmt-package.R)),
  so entry points need `R_init_zusmt`.
- CRAN cares about check *time* and compiled-object size; a large
  vendored solver makes both a standing concern.

## Before a release

This is heading for a first CRAN submission, so the `cran-extrachecks`
skill applies. Stage 9 of `roadmap.md` lists what CRAN will ask about a
package bundling a third-party solver.

## CI

`.github/workflows/R-CMD-check.yaml` calls the reusable workflows in
[pedrobtz/r-actions](https://github.com/pedrobtz/r-actions) (pinned
`@v1`): four runner legs plus three CRAN-like r-devel containers
(`clang23`, `ubuntu-clang`, `ubuntu-gcc16`), all `--as-cran`, **failing
on a WARNING**. A local `devtools::check()` passing is not the bar; the
container legs run compilers no runner has.

The workflow sets `_R_CHECK_BASHISMS_=false` because that check WARNs
when `checkbashisms` is simply missing from the runner. The
`configure-scripts` job runs `checkbashisms --posix` and
`shellcheck -s sh` against every shipped shell script instead — keep new
shell code POSIX and run
`shellcheck -s sh configure configure.win cleanup tools/gmp.sh` locally
before pushing.

`roadmap.md` records which r-actions workflows to adopt at which stage;
do not add them early.

Reading a CI build log: `rcmdcheck` prints the install log only on
failure. The container legs upload a `check-<name>` artifact holding
`zusmt.Rcheck/00install.out` (which contains `configure`’s output and
every compiler line); the runner legs upload only on failure. So:
`gh run download <run-id> -n check-clang23`.
