## Submission

First submission of zusmt.

## R CMD check results

0 errors | 0 warnings | 1 note

```
* checking installed package size ... NOTE
  installed size is 56.1Mb
    sub-directories of 1Mb or more:
      libs  55.9Mb
```

The package bundles the OpenSMT satisfiability modulo theories solver (87
translation units of C++), which is the whole point of the package: it exists
so that solving does not require a separately installed solver binary. The
size is almost entirely the compiled solver in `libs/`, and there is no
subset of it that could be dropped while leaving a working solver.

**The source package is 0.5Mb, and 96% of the installed size is debug
information.** Measuring the sections of the 62.0Mb `zusmt.so` produced by the
GCC container:

```
   34.9Mb  .debug_info
   11.1Mb  .debug_loclists
    6.5Mb  .debug_str
    6.8Mb  other .debug_* sections
  -------
   59.3Mb  debug sections  (96%)
    2.7Mb  everything else, of which .text is 1.9Mb
```

That 2.7Mb is the real footprint of the compiled solver, and it agrees with
the platforms that do not carry DWARF in the shared object: the same sources
install at 2.5Mb on macOS, and the r-devel clang container reports no size
note at all. The difference is `-g` in R's default `CXXFLAGS` applied to 87
translation units of template-heavy C++, not anything the package chooses.

GMP is linked dynamically (`-lgmpxx -lgmp`), so it contributes nothing to the
figure.

We have deliberately not stripped the library or overridden the compiler
flags to reduce this: the flags belong to the builder, and a package that
quietly discards debug information is worse to debug when it does fail on a
platform we cannot test.

## Bundled third-party sources

`src/opensmt/` contains OpenSMT v2.9.2 (commit 34bc1b8), which is MIT
licensed, as is this package. The bundled copyright notices — OpenSMT's and
MiniSat's, which OpenSMT itself bundles — are reproduced in `LICENSE.note`,
and the OpenSMT authors are credited in `Authors@R` as contributors and
copyright holders.

The import is scripted rather than manual (`tools/vendor.sh`), and the version
and commit are recorded in `tools/vendor/manifest.tsv`. The vendored sources
carry a small set of mechanical patches, applied by `tools/patches.sh`, so
that the solver behaves as a library inside R rather than as a command-line
program:

* console output goes through `Rprintf`/`REprintf` rather than to `stdout` or
  `stderr` directly;
* `exit()` and `abort()` are replaced by C++ exceptions, which the package's
  entry points convert into R conditions;
* the system random number generator is replaced by a self-contained one, so
  the package does not touch R's RNG stream;
* the solver's own search-termination check also asks whether R has a pending
  interrupt, so a long solve can be interrupted.

Each patch is a rule that asserts it changed something, so that a future
version of OpenSMT in which a call site has moved fails the re-import rather
than silently leaving the old behaviour in place.

## Method references

The bundled solver is described in the reference given in `Description`. There
is no separate publication describing this R interface.

## Test environments

Checked with `--as-cran` on:

* macOS (release), Windows (release), Ubuntu (release and oldrel-1) — GitHub
  Actions runners
* Debian/Ubuntu containers matching CRAN's r-devel flavours: clang 23,
  clang 22 and GCC 16
* R built `--disable-long-double` (NOLD)

The compiled code is additionally checked under AddressSanitizer and
UndefinedBehaviorSanitizer (on both clang and GCC containers), valgrind,
gctorture, rchk and link-time optimisation. The `nosuggests` flavour is also
checked.
