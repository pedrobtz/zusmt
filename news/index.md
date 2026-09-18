# Changelog

## zusmt 0.1.0

- First release.

- Documentation: a README and a
  [`vignette("zusmt")`](https://pedrobtz.github.io/zusmt/articles/zusmt.md)
  built around problems that run, a pkgdown reference index, and
  `.github/CONTRIBUTING.md` covering how to bump the bundled solver.

- Bundles OpenSMT v2.9.2 (commit `34bc1b8`, 2025-06-16).

- New
  [`smt_logics()`](https://pedrobtz.github.io/zusmt/reference/smt_logics.md)
  reports the supported logics, read from the single definition in the
  C++ layer.

- All eight supported logics are now tested with both a satisfiable and
  an unsatisfiable problem, and a regression corpus of SMT-LIB files
  ships in `inst/smt2/`. Coverage, `nosuggests` and `nold` run in CI.

- Memory and UB checks (`sanitizers`, `valgrind`, `gctorture`, `rchk`,
  `lto`) now run in CI.

- First public API:
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md),
  [`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md),
  [`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md),
  [`smt_model()`](https://pedrobtz.github.io/zusmt/reference/smt_model.md)
  and
  [`smt_release()`](https://pedrobtz.github.io/zusmt/reference/smt_release.md).
  Problems are written in SMT-LIB2 and results come back as R values – a
  named list for models, with exact rationals attached to the doubles.
  Solver diagnostics are raised as R conditions rather than printed.

- Solver handles are external pointers with finalizers, every entry
  point runs behind one exception firewall, and a long search can be
  interrupted: the solver’s own `okContinue()` is asked about pending R
  interrupts, so the search unwinds through its destructors instead of
  being longjmped over.

- The bundled solver is now compiled and linked, and decides problems
  from R. `src/Makevars.in`’s object list is derived from upstream’s own
  CMakeLists by `tools/objects.sh`, and `tools/patches.sh` rewrites
  upstream’s console output, RNG and process-exit calls to things a
  package may do.

- Bundled OpenSMT v2.9.2 under `src/opensmt/` (238 files), imported by
  `tools/vendor.sh`. Nothing compiles it yet – that is the next step.

- Added
  [`smt_toolchain()`](https://pedrobtz.github.io/zusmt/reference/smt_toolchain.md),
  a build-time probe reporting the C++20 compiler, GMP version and
  thread support the package was compiled against. This is Stage 1 of
  `roadmap.md`: it establishes that R’s toolchain can build what OpenSMT
  requires, ahead of vendoring the solver itself.

- Added `configure` / `configure.win`, which locate GMP and generate
  `src/Makevars`.
