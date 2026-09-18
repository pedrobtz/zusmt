# The logics this package supports

The supported set is defined once, in the C++ layer, and read from there
— so this function, the solver's own check and the package's tests
cannot disagree about it.

## Usage

``` r
smt_logics()
```

## Value

A character vector of SMT-LIB logic names accepted by
[`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md).

## Examples

``` r
smt_logics()
#> [1] "QF_UF"    "QF_LIA"   "QF_LRA"   "QF_UFLIA" "QF_UFLRA" "QF_IDL"   "QF_RDL"  
#> [8] "QF_AX"   
```
