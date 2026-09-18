# The model of a satisfiable problem

The model of a satisfiable problem

## Usage

``` r
smt_model(solver)
```

## Arguments

- solver:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md),
  on which
  [`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)
  has just returned `"sat"`.

## Value

A named list, one element per 0-ary declaration. Booleans come back as
logicals and numbers as doubles carrying an `"exact"` attribute with the
solver's exact rational, since an SMT rational need not be representable
as a double. Values of other sorts come back as the solver's own printed
form.

Functions of arity greater than zero are omitted: they have no single
value to report.

## Examples

``` r
s <- smt_solver("QF_LIA")
smt_assert(s, "(declare-const x Int) (assert (= x 42))")
smt_check(s)
#> [1] "sat"
smt_model(s)
#> $x
#> [1] 42
#> attr(,"exact")
#> [1] "42"
#> 
```
