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

The solver, invisibly, so calls can be chained. Anything the script
prints — the output of `get-model`, `get-value`, `get-info` or `echo`,
for instance — is written to the console.

## Details

Use
[`smt_check()`](https://pedrobtz.github.io/zusmt/reference/smt_check.md)
rather than a `(check-sat)` command: it returns the result to R instead
of printing it.

## Errors

The commands in `text` run in order, and the first that fails stops the
script: the commands before it have taken effect, the ones after it have
not, and the error message names the failing command. Nothing is rolled
back – in particular a declaration before the failure remains.

Every error inherits from class `zusmt_error`, after a more specific
one:

- `zusmt_syntax_error`: the text does not parse. Nothing has run.

- `zusmt_smtlib_error`: the solver rejected a command, such as an
  assertion using an undeclared name.

- `zusmt_unsupported_input`: an assertion is outside what the solver's
  logic can decide – a non-difference constraint under `QF_IDL` or
  `QF_RDL`. The assertion is not added.

- `zusmt_input_too_deep`: the text nests more than 10,000 levels of
  parentheses, which would overflow the C stack. Nothing has run.

## Differences from SMT-LIB

These come from the bundled solver, and are worth knowing when running a
script written for another one.

- `pop` discards assertions but not declarations: a constant declared
  after a `push` is still declared after the matching `pop`.

- `(get-value ...)`, `(get-model)` and `(get-info ...)` print their
  answer rather than returning it; use
  [`smt_model()`](https://pedrobtz.github.io/zusmt/reference/smt_model.md)
  for values. Constants introduced with `define-fun` do not appear in
  [`smt_model()`](https://pedrobtz.github.io/zusmt/reference/smt_model.md).

- `(reset)` is not supported, nor are `(get-info :name)` and
  `(get-info :version)`, nor the `abs` and `to_real` functions.

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
