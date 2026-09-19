# The unsat core of an unsatisfiable problem

The subset of the assertions that is already unsatisfiable on its own –
the solver's explanation of why the answer was `"unsat"`.

## Usage

``` r
smt_unsat_core(solver, named_only = FALSE)
```

## Arguments

- solver:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md),
  created with `unsat_cores = TRUE`, on which
  [`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)
  has returned `"unsat"`.

- named_only:

  Whether to report only assertions named with `(! ... :named n)`, which
  is what SMT-LIB means by an unsat core and what the `(get-unsat-core)`
  command returns. `FALSE`, the default, reports the core's assertions
  whether or not they were named – an SMT-LIB-conformant core of a
  script that names nothing is empty, which is rarely the answer an R
  caller wants.

## Value

A character vector of assertions in the solver's own printed form, or of
names when `named_only = TRUE`. If any assertion in the core was named,
the names come back as the vector's
[`names()`](https://rdrr.io/r/base/names.html).

## Details

Requires a solver created with `smt_solver(unsat_cores = TRUE)`: whether
a proof is recorded is fixed when the solver is built, so it cannot be
turned on after the fact.

## See also

[`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md),
[`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)

## Examples

``` r
s <- smt_solver("QF_LIA", unsat_cores = TRUE)
smt_assert(s, "
  (declare-const x Int)
  (assert (! (> x 5) :named lower))
  (assert (! (< x 3) :named upper))
  (assert (! (= x 4) :named unrelated))
")
smt_check(s)
#> [1] "unsat"
smt_unsat_core(s)
#>                          lower                          upper 
#> "(not (<= (- 5) (* (- 1) x)))"               "(not (<= 3 x))" 
smt_unsat_core(s, named_only = TRUE)
#> [1] "lower" "upper"
```
