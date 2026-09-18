# Release a solver

Frees the solver's memory without waiting for garbage collection. Using
the solver afterwards is an error rather than a crash.

## Usage

``` r
smt_release(solver)
```

## Arguments

- solver:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md).

## Value

`NULL`, invisibly.

## Examples

``` r
s <- smt_solver()
smt_release(s)
```
