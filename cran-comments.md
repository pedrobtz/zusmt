## Submission

First submission of zusmt.

## R CMD check results

0 errors | 0 warnings | 1 note

```
* checking installed package size ... NOTE
  installed size is 44.9Mb
    sub-directories of 1Mb or more:
      libs  44.8Mb
```

The package bundles the OpenSMT satisfiability modulo theories solver (87
translation units of C++), which is the whole point of the package: it exists
so that solving does not require a separately installed solver binary. The
size is almost entirely the compiled solver in `libs/`, and there is no
subset of it that could be dropped while leaving a working solver.

The figure above is what the check reports on the Linux builders; the same
sources install at about 2.4Mb on macOS.

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

The compiled code is additionally checked under valgrind, gctorture, rchk and
link-time optimisation.
