# Solving problems with zusmt

An SMT solver answers one question: is there an assignment of values to
these variables that makes all of these statements true at once? It
differs from an optimiser in that it looks for *any* solution rather
than the best one, and from a constraint solver in that the statements
may involve several theories — integers, reals, uninterpreted functions,
arrays — reasoned about together.

``` r

library(zusmt)
```

## Asking a question

A solver is created for a logic, given assertions, and then asked to
check them.

``` r

s <- smt_solver("QF_LIA")
smt_assert(s, "
  (declare-const x Int)
  (declare-const y Int)
  (assert (> x 3))
  (assert (< x 7))
  (assert (= y (* 2 x)))
")

smt_check(s)
#> [1] "sat"
```

`"sat"` means a solution exists, and
[`smt_model()`](https://pedrobtz.github.io/zusmt/reference/smt_model.md)
produces one.

``` r

smt_model(s)
#> $x
#> [1] 4
#> attr(,"exact")
#> [1] "4"
#> 
#> $y
#> [1] 8
#> attr(,"exact")
#> [1] "8"
```

The answer is *a* solution, not *the* solution: any `x` strictly between
3 and 7 would do. If you need a particular one, constrain it further.

## When there is no solution

``` r

u <- smt_solver("QF_LIA")
smt_assert(u, "(declare-const x Int) (assert (= (* 2 x) 3))")
smt_check(u)
#> [1] "unsat"
```

`2x = 3` has no integer solution. Note that the same assertion is
satisfiable over the reals, which is the difference between `QF_LIA` and
`QF_LRA` rather than a detail of how the problem is written:

``` r

r <- smt_solver("QF_LRA")
smt_assert(r, "(declare-const x Real) (assert (= (* 2.0 x) 3.0))")
smt_check(r)
#> [1] "sat"
smt_model(r)$x
#> [1] 1.5
#> attr(,"exact")
#> [1] "3/2"
```

Asking for a model when the answer was `"unsat"` is an error rather than
an empty list, because there is nothing to report:

``` r

smt_model(u)
#> Error in `smt_model()`:
#> ! a model is only available after a satisfiable check
```

## Why there is no solution

`"unsat"` on its own does not say which assertions are to blame. A
solver created with `unsat_cores = TRUE` can report the subset that
already conflicts, which is usually a small part of a large problem:

``` r

conflict <- smt_solver("QF_LIA", unsat_cores = TRUE)
smt_assert(conflict, "
  (declare-const x Int)
  (declare-const y Int)
  (assert (! (> x 10) :named x-large))
  (assert (! (< x 5)  :named x-small))
  (assert (! (= y 1)  :named y-fixed))
")
smt_check(conflict)
#> [1] "unsat"
smt_unsat_core(conflict)
#>                         x-large                         x-small 
#> "(not (<= (- 10) (* (- 1) x)))"                "(not (<= 5 x))"
```

`y-fixed` is absent because it is not part of the contradiction – that
is the point of a core. Name assertions with `(! ... :named n)` and the
names come back with them.

The argument goes on
[`smt_solver()`](https://pedrobtz.github.io/zusmt/reference/smt_solver.md)
rather than being set later because OpenSMT decides whether to record a
proof when it builds the solver. Asking afterwards cannot work, and is
refused:

``` r

smt_assert(smt_solver("QF_LIA"), "(set-option :produce-unsat-cores true)")
#> Error in `smt_assert()`:
#> ! (error "set-option failed for :produce-unsat-cores: Option cannot be changed at this point")
```

## Summarising a conflict

An unsat core says *which* assertions conflict. An interpolant says
*what* the conflict is, in the vocabulary the two sides share.

Split the assertions into a group `A` and everything else. A Craig
interpolant follows from `A`, contradicts the rest, and mentions only
symbols both sides use:

``` r

i <- smt_solver("QF_LIA", interpolants = TRUE)
smt_assert(i, "
  (declare-const x Int)
  (declare-const y Int)
  (declare-const z Int)
  (assert (! (and (> x 5) (= y x)) :named A))
  (assert (! (and (< z 3) (= z y)) :named B))
")
smt_check(i)
#> [1] "unsat"
smt_interpolant(i, "A")
#> [1] "(<= 6 y)"
```

`A` forces `y` to be at least 6 and the other group forces it below 3.
The interpolant says just that, and says it about `y` alone – `x`
belongs to `A`, `z` to the other side, and neither appears. That is the
useful part: it explains the contradiction without either group’s
private detail, which is what makes interpolants a building block for
abstraction and invariant generation.

## Exact values

Solvers compute in rationals. Many rationals are not doubles, so a model
value carries both: the `numeric` you can compute with, and the solver’s
exact answer as an attribute.

``` r

third <- smt_solver("QF_LRA")
smt_assert(third, "(declare-const v Real) (assert (= (* 3.0 v) 1.0))")
smt_check(third)
#> [1] "sat"

value <- smt_model(third)$v
value
#> [1] 0.3333333
#> attr(,"exact")
#> [1] "1/3"
attr(value, "exact")
#> [1] "1/3"
```

Whether that distinction matters depends on what you do next. It matters
if you feed the answer back into another constraint.

## Beyond arithmetic

Uninterpreted functions let you state that something *is a function* —
equal inputs give equal outputs — without saying which function. That
alone is enough to make some problems unsatisfiable:

``` r

uf <- smt_solver("QF_UF")
smt_assert(uf, "
  (declare-sort U 0)
  (declare-fun f (U) U)
  (declare-const a U)
  (declare-const b U)
  (assert (= a b))
  (assert (distinct (f a) (f b)))
")

smt_check(uf)
#> [1] "unsat"
```

No definition of `f` can satisfy that, whatever `U` and `f` are.

Arrays work similarly, with `select` and `store` and the rule that
reading back what you just wrote gives what you wrote:

``` r

ax <- smt_solver("QF_AX")
smt_assert(ax, "
  (declare-sort I 0)
  (declare-sort E 0)
  (declare-const arr (Array I E))
  (declare-const i I)
  (declare-const e E)
  (assert (distinct (select (store arr i e) i) e))
")

smt_check(ax)
#> [1] "unsat"
```

## Building up a problem

Assertions accumulate, so a problem can be built across several calls —
and `push`/`pop` let you explore an assumption and then discard it.

``` r

k <- smt_solver("QF_LIA")
smt_assert(k, "(declare-const n Int) (assert (> n 10))")
smt_check(k)
#> [1] "sat"

smt_assert(k, "(push 1) (assert (< n 5))")
smt_check(k)
#> [1] "unsat"

smt_assert(k, "(pop 1)")
smt_check(k)
#> [1] "sat"
```

[`smt_assert()`](https://pedrobtz.github.io/zusmt/reference/smt_assert.md)
accepts any SMT-LIB2 command, not only `assert`, so a script written for
another solver usually runs unchanged.

## Which logic to ask for

``` r

smt_logics()
#> [1] "QF_UF"    "QF_LIA"   "QF_LRA"   "QF_UFLIA" "QF_UFLRA" "QF_IDL"   "QF_RDL"  
#> [8] "QF_AX"
```

Pick the weakest logic that expresses the problem: the solver can use a
specialised decision procedure, and a mistake in the problem is more
likely to be reported as an error than quietly accepted. `QF_IDL` and
`QF_RDL` are difference logic — constraints of the form `x - y <= c` —
which covers scheduling and similar problems and is decided faster than
general arithmetic.

## Errors

Mistakes in the input become R conditions rather than printed output, so
they can be caught:

``` r

bad <- smt_solver("QF_LIA")
smt_assert(bad, "(assert (> undeclared 1))")
#> Error in `smt_assert()`:
#> ! (error "Unknown symbol `undeclared '")
#> 
#> (error "assertion returns an unknown sort")
```

## Bounding a solve

Satisfiability is decidable for these logics but not cheap: a problem a
few lines long can take minutes, and the bundled solver has no time
limit of its own.
[`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)
takes one.

``` r

s <- smt_solver("QF_LIA")
smt_assert(s, "(declare-const x Int) (assert (> x 3))")
smt_check(s, timeout = 30)
#> [1] "sat"
```

A solve that runs out of time reports `"unknown"` – it has not decided
anything – and warns, so a bounded solve is never silently mistaken for
a solver that gave up on its own. Catch the warning by its class if you
want to handle the two differently:

``` r

withCallingHandlers(
  smt_check(s, timeout = 5),
  zusmt_timeout = function(cond) {
    message("giving up on this one")
    invokeRestart("muffleWarning")
  }
)
```

The bound is checked where the solver decides whether to keep searching,
so treat it as a floor on when the call returns rather than a hard
guarantee: a solve can overrun it inside preprocessing or a single long
step. Pressing `Ctrl-C` works at the same points.

## Releasing a solver

A solver holds memory in the bundled C++ library, freed when R
garbage-collects the handle. To release it at a known point — a long
loop, a large problem — use
[`smt_release()`](https://pedrobtz.github.io/zusmt/reference/smt_release.md).

``` r

smt_release(s)
```
