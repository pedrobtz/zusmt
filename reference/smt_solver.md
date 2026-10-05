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

  The difference logics, `QF_IDL` and `QF_RDL`, accept only comparisons
  of the form `(op (- x y) c)`, `(op x c)` or `(op x y)`, where `x` and
  `y` are constants and `c` a number, together with `distinct` and
  arithmetic `ite` terms that reduce to those. Any other arithmetic – a
  sum of two variables, a coefficient other than 1 – is refused by
  [`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md)
  with an error of class `zusmt_unsupported_input`; use `QF_LIA` or
  `QF_LRA` for it.

- unsat_cores:

  Whether to record enough of the search to report an unsat core with
  [`smt_unsat_core()`](https://pedrobtz.github.io/zusmt/reference/smt_unsat_core.md).
  Costs time and memory on every solve, so it is off by default.

- interpolants:

  Whether to enable Craig interpolation, for
  [`smt_interpolant()`](https://pedrobtz.github.io/zusmt/reference/smt_interpolant.md).

  This changes how integer problems are solved, not only what is
  recorded: OpenSMT stops deriving cuts from its proofs, which it cannot
  interpolate, so a `QF_LIA` problem decided instantly without
  interpolation may not terminate with it. Pass a `timeout` to
  [`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)
  on such solvers.

## Value

A solver handle, to be passed to the other `smt_*()` functions.

`unsat_cores` and `interpolants` are arguments here, rather than options
to set later with
[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md),
because the solver decides whether to record a proof when it is built.
Setting them afterwards cannot work, and the solver rejects the attempt.

Each solver holds memory in the bundled C++ library – on the order of
100 KB even when empty – that R's garbage collector does not see, so a
loop creating many solvers can accumulate far more than R's own memory
use suggests. Call
[`smt_release()`](https://pedrobtz.github.io/zusmt/reference/smt_release.md)
on each one when it is done.

## See also

[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md),
[`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md),
[`smt_model()`](https://pedrobtz.github.io/zusmt/reference/smt_model.md),
[`smt_unsat_core()`](https://pedrobtz.github.io/zusmt/reference/smt_unsat_core.md),
[`smt_interpolant()`](https://pedrobtz.github.io/zusmt/reference/smt_interpolant.md)

## Examples

``` r
s <- smt_solver("QF_LIA")
smt_assert(s, "(declare-const x Int) (assert (> x 3))")
smt_check(s)
#> [1] "sat"
```
