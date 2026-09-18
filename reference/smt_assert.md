# Send SMT-LIB input to a solver

Runs SMT-LIB2 text through the bundled solver. Despite the name, any
SMT-LIB2 commands are accepted, not only `assert` — declarations, `push`
and `pop`, options, and so on — because the package is a front end to
the solver's own language rather than a curated subset of it.

## Usage

``` r
smt_assert(solver, text)
```

## Arguments

- solver:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md).

- text:

  A single string of SMT-LIB2 input. Newlines are fine, and several
  commands may appear in one call.

## Value

The solver, invisibly, so calls can be chained.

## Details

Use
[`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)
rather than a `(check-sat)` command: it returns the result to R instead
of printing it.

## Examples

``` r
s <- smt_solver("QF_LRA")
smt_assert(s, "
  (declare-const x Real)
  (declare-const y Real)
  (assert (> x y))
  (assert (> y 0.0))
")
smt_check(s)
#> [1] "sat"
```
