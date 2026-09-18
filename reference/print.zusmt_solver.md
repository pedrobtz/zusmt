# Print a solver

Print a solver

## Usage

``` r
# S3 method for class 'zusmt_solver'
print(x, ...)
```

## Arguments

- x:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md).

- ...:

  Ignored, for compatibility with
  [`print()`](https://rdrr.io/r/base/print.html).

## Value

`x`, invisibly. Called for the side effect of printing the solver's
logic and whether it has been released.

## Examples

``` r
print(smt_solver("QF_LIA"))
#> <zusmt solver: QF_LIA>
```
