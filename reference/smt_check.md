# Check satisfiability

Check satisfiability

## Usage

``` r
smt_check(solver, timeout = Inf)
```

## Arguments

- solver:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md).

- timeout:

  Seconds to allow the search, or `Inf` for no limit. On expiry the
  result is `"unknown"` and a warning of class `zusmt_timeout` is
  signalled.

  The bound is honoured where the solver checks whether to keep
  searching, which is its SAT loop, so a solve can overrun it while
  inside preprocessing or a single long theory propagation. Treat it as
  a floor on when the call returns rather than a hard guarantee.

## Value

`"sat"`, `"unsat"` or `"unknown"`.

A timeout reports `"unknown"`, since a solve that ran out of time has
not decided anything – the warning is what distinguishes it from a
solver that gave up on its own.

A long search can be interrupted. Because the poll that notices a
pending interrupt also consumes it, the interrupt is re-signalled from R
as a condition inheriting from `interrupt`, so `tryCatch(interrupt = )`
works; with no handler it becomes an error rather than returning a
value.

## Examples

``` r
s <- smt_solver("QF_LIA")
smt_assert(s, "(declare-const x Int) (assert (> x 0)) (assert (< x 0))")
smt_check(s)
#> [1] "unsat"

# A bounded solve on a problem that is hard rather than large.
smt_check(s, timeout = 10)
#> [1] "unsat"
```
