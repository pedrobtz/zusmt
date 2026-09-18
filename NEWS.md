# zusmt 0.0.0.9000

* Added `smt_toolchain()`, a build-time probe reporting the C++20 compiler,
  GMP version and thread support the package was compiled against. This is
  Stage 1 of `roadmap.md`: it establishes that R's toolchain can build what
  OpenSMT requires, ahead of vendoring the solver itself.
* Added `configure` / `configure.win`, which locate GMP and generate
  `src/Makevars`.
