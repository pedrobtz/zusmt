
# zusmt

<!-- badges: start -->
[![R-CMD-check](https://github.com/pedrobtz/zusmt/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/pedrobtz/zusmt/actions/workflows/R-CMD-check.yaml)
[![native-checks](https://github.com/pedrobtz/zusmt/actions/workflows/native-checks.yml/badge.svg)](https://github.com/pedrobtz/zusmt/actions/workflows/native-checks.yml)
![Coverage](https://github.com/pedrobtz/zusmt/raw/main/.github/badges/coverage.svg)
<!-- badges: end -->

An SMT solver in R. zusmt bundles [OpenSMT](https://github.com/usi-verification-and-security/opensmt)
and exposes it through a small R interface: you write problems in SMT-LIB2 and get answers back as R
values. There is no separate solver to install and nothing to configure — the solver is compiled into
the package.

## Installation

``` r
install.packages("zusmt")
```

Or the development version:

``` r
# install.packages("pak")
pak::pak("pedrobtz/zusmt")
```

Building from source needs a C++20 compiler and [GMP](https://gmplib.org/) with its C++ bindings
(`libgmp-dev` on Debian and Ubuntu, `gmp-devel` on Fedora, `brew install gmp` on macOS; Rtools
supplies it on Windows).

## Example

Three people need distinct meeting slots between 9 and 17, Alice before Bob, and Carol two hours
after Bob:

``` r
library(zusmt)

s <- smt_solver("QF_LIA")
smt_assert(s, "
  (declare-const alice Int)
  (declare-const bob   Int)
  (declare-const carol Int)
  (assert (and (>= alice 9) (<= alice 17)))
  (assert (and (>= bob   9) (<= bob   17)))
  (assert (and (>= carol 9) (<= carol 17)))
  (assert (distinct alice bob carol))
  (assert (< alice bob))
  (assert (= carol (+ bob 2)))
")

smt_check(s)
#> [1] "sat"

smt_model(s)
#> $alice
#> [1] 9
#> $bob
#> [1] 10
#> $carol
#> [1] 12
```

When a problem has no solution, you are told so rather than given a wrong answer. `2x = 3` has no
integer solution:

``` r
u <- smt_solver("QF_LIA")
smt_assert(u, "(declare-const x Int) (assert (= (* 2 x) 3))")
smt_check(u)
#> [1] "unsat"
```

## Exact values

Solvers work in rationals, and not every rational is a double. Model values carry the solver's exact
answer alongside the `numeric` one, so you can compute with the double and still see what was meant:

``` r
r <- smt_solver("QF_LRA")
smt_assert(r, "(declare-const third Real) (assert (= (* 3.0 third) 1.0))")
smt_check(r)
#> [1] "sat"

value <- smt_model(r)$third
value
#> [1] 0.3333333
attr(value, "exact")
#> [1] "1/3"
```

## Logics

``` r
smt_logics()
#> [1] "QF_UF"    "QF_LIA"   "QF_LRA"   "QF_UFLIA" "QF_UFLRA" "QF_IDL"   "QF_RDL"
#> [8] "QF_AX"
```

Uninterpreted functions, linear integer and real arithmetic and their combinations, difference logic,
and arrays. `smt_assert()` accepts any SMT-LIB2 command, not only `assert` — `push`, `pop`,
declarations and options all work, so a script written for another solver usually runs unchanged.

A long search can be interrupted: `Ctrl-C` stops it and raises an R condition, rather than leaving
the session wedged in the solver.

## What is bundled

OpenSMT v2.9.2, under `src/opensmt/`, imported by `tools/vendor.sh` and recorded in
`tools/vendor/manifest.tsv`. Both zusmt and OpenSMT are MIT licensed; see `LICENSE.note` for the
bundled copyright notices. The vendored sources carry a small set of patches, applied mechanically by
`tools/patches.sh`, so that the solver writes to R's console rather than to stdout, raises exceptions
instead of calling `exit()`, and stops when you interrupt it.
