# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Purpose

`zusmt` is an R package whose reason to exist is to **vendor OpenSMT** (the SMT solver from
usi-verification-and-security/opensmt) and expose it to R. The R layer is a thin wrapper; nearly all
of the work is in `src/` — getting a third-party C++ codebase to build reliably under `R CMD INSTALL`
on Linux, macOS and Windows (see the CI matrix in [R-CMD-check.yaml](.github/workflows/R-CMD-check.yaml)).

## Current state

Stage 1 of [roadmap.md](roadmap.md): no OpenSMT sources are vendored yet. What exists is the build
glue and a probe — `configure`/`configure.win` (sharing `tools/gmp.sh`) locate GMP and generate
`src/Makevars` from `src/Makevars.in`, and `smt_toolchain()` in [src/toolchain.cc](src/toolchain.cc)
reports the compiler, `__cplusplus`, GMP version and a threaded rational round trip. That file is a
probe, not a feature: it goes away when the vendored solver replaces it. Read `roadmap.md` before
starting work — it says which stage the next piece of work belongs to.

## Commands

```r
devtools::load_all()                    # compile src/ + load; the main dev loop
devtools::document()                    # regenerate NAMESPACE + man/ from roxygen
devtools::test()                        # run all tests
devtools::test(filter = "solver")       # run tests/testthat/test-solver.R only
testthat::test_file("tests/testthat/test-solver.R")
devtools::check()                       # full R CMD check, as CI runs it
pkgdown::build_site()                   # site is published from _pkgdown.yml
```

Compiled-code changes are only picked up by a fresh `load_all()`; when Makevars or vendored sources
change, `devtools::clean_dll()` first. `R CMD INSTALL --preclean .` is the reliable fallback.

## Vendoring constraints to respect

- **Sources must be in-tree and self-contained.** CRAN/`R CMD check` builds `src/` with R's own
  toolchain (`Makevars`/`Makevars.win`), not the upstream CMake build. Vendored code is compiled by
  listing its sources, not by invoking CMake at install time; no network access during install.
- **Keep vendored code unmodified where possible.** Put OpenSMT under a clearly separated
  subdirectory of `src/`, keep the upstream license and a record of the imported version/commit
  (`LICENSE.note`, and `src/*/README` or similar), and add it to `.Rbuildignore` only if it must not
  ship. Local patches should be few, documented, and reapplicable when the vendored version bumps.
- **External dependencies are the hard part.** OpenSMT needs C++17 and GMP; anything not bundled must
  be discoverable via `configure`/`configure.win` and declared in `SystemRequirements:` in DESCRIPTION.
  Decide bundle-vs-link before writing build glue — it determines the whole `src/` layout.
- **The R/C++ boundary stays narrow.** Solver state lives behind external pointers with finalizers;
  upstream exceptions must never cross into R unhandled, and upstream must not call `exit()`/`abort()`
  or write to stdout on a check run. Registration goes through `@useDynLib zusmt, .registration = TRUE`
  (already set in [R/zusmt-package.R](R/zusmt-package.R)), so entry points need `R_init_zusmt`.
- CRAN cares about check *time* and compiled-object size; a large vendored solver makes both a
  standing concern.

## Before a release

This is heading for a first CRAN submission, so the `cran-extrachecks` skill applies. Stage 9 of
`roadmap.md` lists what CRAN will ask about a package bundling a third-party solver.

## CI

`.github/workflows/R-CMD-check.yaml` calls the reusable workflows in
[pedrobtz/r-actions](https://github.com/pedrobtz/r-actions) (pinned `@v1`): four runner legs plus
three CRAN-like r-devel containers (`clang23`, `ubuntu-clang`, `ubuntu-gcc16`), all `--as-cran`,
**failing on a WARNING**. A local `devtools::check()` passing is not the bar; the container legs run
compilers no runner has.

The workflow sets `_R_CHECK_BASHISMS_=false` because that check WARNs when `checkbashisms` is simply
missing from the runner. The `configure-scripts` job runs `checkbashisms --posix` and
`shellcheck -s sh` against every shipped shell script instead — keep new shell code POSIX and run
`shellcheck -s sh configure configure.win cleanup tools/gmp.sh` locally before pushing.

`roadmap.md` records which r-actions workflows to adopt at which stage; do not add them early.
