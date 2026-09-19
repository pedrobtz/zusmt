# zusmt 0.1.0

* First release.

* New `smt_interpolant()` returns a Craig interpolant between two groups of
  named assertions, as an R value rather than printed output. Enabling
  `interpolants = TRUE` made `(get-interpolants ...)` reachable; this makes
  its result usable.

* `smt_check()` gains `timeout`, a bound in seconds on a single solve.
  OpenSMT has none of its own -- `:timeout` is not one of its options, so
  `(set-option :timeout n)` is accepted and ignored -- which left an
  unbounded solve able to hang an R session until interrupted. The deadline is
  checked where the interrupt poll already is, in the solver's own
  `okContinue()`, so an expired bound ends the search by unwinding through its
  destructors. A timed-out solve returns `"unknown"` and warns with class
  `zusmt_timeout`; a solve that reaches an answer reports it however late.

* Fixed a crash. `(set-option :produce-unsat-cores true)` on a live solver
  segfaulted the R session as soon as a core was requested: OpenSMT decides
  whether to record a proof when the SAT solver is constructed, and setting
  the option afterwards flipped the flag with no proof behind it. Upstream
  guards `:produce-proofs` and `:produce-interpolants` against exactly this
  and omits `:produce-unsat-cores`; a patch rule adds it, so the attempt is
  now refused rather than fatal.

* New `smt_unsat_core()` reports which assertions are unsatisfiable together,
  and `smt_solver()` gains `unsat_cores` and `interpolants`. They are
  constructor arguments rather than options to set later because that is the
  only point at which they can take effect -- which is also what made unsat
  cores and Craig interpolation unreachable from R before.

* Documentation: a README and a `vignette("zusmt")` built around problems that
  run, a pkgdown reference index, and `.github/CONTRIBUTING.md` covering how to
  bump the bundled solver.
* Bundles OpenSMT v2.9.2 (commit `34bc1b8`, 2025-06-16).

* New `smt_logics()` reports the supported logics, read from the single
  definition in the C++ layer.
* All eight supported logics are now tested with both a satisfiable and an
  unsatisfiable problem, and a regression corpus of SMT-LIB files ships in
  `inst/smt2/`. Coverage, `nosuggests` and `nold` run in CI.

* Memory and UB checks (`sanitizers`, `valgrind`, `gctorture`, `rchk`, `lto`) now run in CI.
* First public API: `smt_solver()`, `smt_assert()`, `smt_check()`, `smt_model()`
  and `smt_release()`. Problems are written in SMT-LIB2 and results come back as
  R values -- a named list for models, with exact rationals attached to the
  doubles. Solver diagnostics are raised as R conditions rather than printed.

* Solver handles are external pointers with finalizers, every entry point runs
  behind one exception firewall, and a long search can be interrupted: the
  solver's own `okContinue()` is asked about pending R interrupts, so the
  search unwinds through its destructors instead of being longjmped over.

* The bundled solver is now compiled and linked, and decides problems from R.
  `src/Makevars.in`'s object list is derived from upstream's own CMakeLists by
  `tools/objects.sh`, and `tools/patches.sh` rewrites upstream's console
  output, RNG and process-exit calls to things a package may do.

* Bundled OpenSMT v2.9.2 under `src/opensmt/` (238 files), imported by
  `tools/vendor.sh`. Nothing compiles it yet -- that is the next step.

* Added `smt_toolchain()`, a build-time probe reporting the C++20 compiler,
  GMP version and thread support the package was compiled against. This is
  Stage 1 of `roadmap.md`: it establishes that R's toolchain can build what
  OpenSMT requires, ahead of vendoring the solver itself.
* Added `configure` / `configure.win`, which locate GMP and generate
  `src/Makevars`.
