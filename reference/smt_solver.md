# Create a solver

Creates a solver for one of the SMT-LIB logics, ready to be given
assertions with
[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md).
The solver holds state: assertions accumulate until the handle is
released or garbage collected.

## Usage

``` r
smt_solver(logic = "QF_UF")
```

## Arguments

- logic:

  An SMT-LIB logic name. One of `"QF_UF"` (uninterpreted functions),
  `"QF_LIA"` / `"QF_LRA"` (linear integer / real arithmetic),
  `"QF_UFLIA"`, `"QF_UFLRA"`, `"QF_IDL"`, `"QF_RDL"` or `"QF_AX"`
  (arrays).

## Value

A solver handle, to be passed to the other `smt_*()` functions.

## See also

[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md),
[`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md),
[`smt_model()`](https://pedrobtz.github.io/zusmt/reference/smt_model.md)

## Examples

``` r
s <- smt_solver("QF_LIA")
smt_assert(s, "(declare-const x Int) (assert (> x 3))")
smt_check(s)
#> [1] "sat"
```
