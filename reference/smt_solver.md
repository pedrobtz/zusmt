# Create a solver

Creates a solver for one of the SMT-LIB logics, ready to be given
assertions with
[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md).
The solver holds state: assertions accumulate until the handle is
released or garbage collected.

## Usage

``` r
smt_solver(logic = "QF_UF", unsat_cores = FALSE, interpolants = FALSE)
```

## Arguments

- logic:

  An SMT-LIB logic name;
  [`smt_logics()`](https://pedrobtz.github.io/zusmt/reference/smt_logics.md)
  returns the ones this package supports. They cover uninterpreted
  functions, linear integer and real arithmetic, their combinations,
  difference logic and arrays.

- unsat_cores:

  Whether to record enough of the search to report an unsat core with
  [`smt_unsat_core()`](https://pedrobtz.github.io/zusmt/reference/smt_unsat_core.md).
  Costs time and memory on every solve, so it is off by default.

- interpolants:

  Whether to enable Craig interpolation, after which
  `(get-interpolants ...)` can be sent with
  [`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md).
  There is no dedicated R function for it yet; the output is printed
  rather than returned.

## Value

A solver handle, to be passed to the other `smt_*()` functions.

`unsat_cores` and `interpolants` are arguments here, rather than options
to set later with
[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md),
because the solver decides whether to record a proof when it is built.
Setting them afterwards cannot work, and the solver rejects the attempt.

## See also

[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md),
[`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md),
[`smt_model()`](https://pedrobtz.github.io/zusmt/reference/smt_model.md),
[`smt_unsat_core()`](https://pedrobtz.github.io/zusmt/reference/smt_unsat_core.md)

## Examples

``` r
s <- smt_solver("QF_LIA")
smt_assert(s, "(declare-const x Int) (assert (> x 3))")
smt_check(s)
#> [1] "sat"
```
