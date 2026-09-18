# Check satisfiability

Check satisfiability

## Usage

``` r
smt_check(solver)
```

## Arguments

- solver:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md).

## Value

`"sat"`, `"unsat"` or `"unknown"`.

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
```
