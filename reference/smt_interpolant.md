# A Craig interpolant between two groups of assertions

Given assertions that are unsatisfiable together, split into a group `A`
and everything else `B`, an interpolant is a formula that follows from
`A`, contradicts `B`, and mentions only the symbols `A` and `B` share.
It is a summary of why the two conflict, in their shared vocabulary –
which is what makes it useful for abstraction and for explaining a
contradiction in terms neither side owns alone.

## Usage

``` r
smt_interpolant(solver, a)
```

## Arguments

- solver:

  A solver from
  [`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md),
  created with `interpolants = TRUE`, on which
  [`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)
  has returned `"unsat"` and to which nothing has been asserted since
  (otherwise an error of class `zusmt_stale_result`).

- a:

  Names of the assertions forming group `A`, as given with
  `(! ... :named n)`. Every other assertion forms `B`.

## Value

A character vector of interpolants as SMT-LIB terms, usually of length
one. Negative and fractional numbers are written the SMT-LIB way,
`(- 1)` and `(/ 16 5)`, so the result can be given to another solver.

## Details

Requires a solver created with `smt_solver(interpolants = TRUE)`, since
whether a proof is recorded is fixed when the solver is built. Enabling
it can make `QF_LIA` checks much slower; see
[`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md).

The bundled solver interpolates `QF_UF`, `QF_LIA` and `QF_LRA` problems,
and only some `QF_IDL` and `QF_RDL` ones. For the other logics, and for
the difference-logic problems it cannot handle, the result is an error
of class `zusmt_unsupported_input`.

## See also

[`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md),
[`smt_unsat_core()`](https://pedrobtz.github.io/zusmt/reference/smt_unsat_core.md)

## Examples

``` r
s <- smt_solver("QF_LIA", interpolants = TRUE)
smt_assert(s, "
  (declare-const x Int)
  (declare-const y Int)
  (assert (! (and (> x 5) (= y x)) :named A))
  (assert (! (< y 3) :named B))
")
smt_check(s)
#> [1] "unsat"
smt_interpolant(s, "A")
#> [1] "(<= 6 y)"
```
