# Changelog

## zusmt 0.0.0.9000

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
