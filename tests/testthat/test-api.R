test_that("a satisfiable problem is solved and its model returned", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, "
    (declare-const x Int)
    (declare-const y Int)
    (assert (> x 3))
    (assert (< x 7))
    (assert (= y (* 2 x)))
  ")

  expect_identical(smt_check(s), "sat")

  m <- smt_model(s)
  expect_named(m, c("x", "y"), ignore.order = TRUE)
  expect_gt(m$x, 3)
  expect_lt(m$x, 7)
  # ignore_attr: model values carry an "exact" attribute, and arithmetic on
  # the right-hand side drops it.
  expect_equal(m$y, 2 * m$x, ignore_attr = TRUE)
})

test_that("an unsatisfiable problem reports unsat and refuses a model", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const z Int) (assert (> z 0)) (assert (< z 0))")

  expect_identical(smt_check(s), "unsat")
  expect_error(smt_model(s), "satisfiable")
})

test_that("rational values keep their exact form alongside the double", {
  s <- smt_solver("QF_LRA")
  smt_assert(s, "(declare-const r Real) (assert (= (* 3.0 r) 1.0))")
  expect_identical(smt_check(s), "sat")

  r <- smt_model(s)$r
  # 1/3 is not representable as a double, which is the whole reason the exact
  # value travels with it.
  expect_equal(r, 1 / 3, ignore_attr = TRUE)
  expect_identical(attr(r, "exact"), "1/3")
})

test_that("booleans come back as logicals", {
  s <- smt_solver("QF_UF")
  smt_assert(s, "(declare-const p Bool) (assert p)")
  expect_identical(smt_check(s), "sat")
  expect_true(smt_model(s)$p)
})

test_that("assertions accumulate across calls", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const n Int)")
  smt_assert(s, "(assert (> n 10))")
  expect_identical(smt_check(s), "sat")

  smt_assert(s, "(assert (< n 5))")
  expect_identical(smt_check(s), "unsat")
})

test_that("push and pop work, since any SMT-LIB command is accepted", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const k Int) (assert (> k 0))")
  smt_assert(s, "(push 1) (assert (< k 0))")
  expect_identical(smt_check(s), "unsat")

  smt_assert(s, "(pop 1)")
  expect_identical(smt_check(s), "sat")
})

test_that("a syntax error becomes an R error rather than printed output", {
  s <- smt_solver("QF_LIA")

  # Unbalanced parentheses: the parser rejects it. The parser's own diagnostic
  # has to arrive *in the condition* -- it used to print to the console and
  # raise a separate, contentless error, which is two half-answers.
  expect_error(smt_assert(s, "(assert (> x"), "syntax error")

  # And nothing may reach the console alongside it.
  expect_silent(try(smt_assert(s, "(assert (> x"), silent = TRUE))
})

test_that("a semantic error becomes an R error too", {
  s <- smt_solver("QF_LIA")
  # Parses, but the symbol was never declared.
  expect_error(smt_assert(s, "(assert (> nope 1))"), "Unknown symbol")
})

test_that("functions of arity > 0 are omitted from the model", {
  s <- smt_solver("QF_UF")
  smt_assert(s, "
    (declare-sort U 0)
    (declare-fun f (U) U)
    (declare-const a U)
    (assert (distinct (f a) a))
  ")
  expect_identical(smt_check(s), "sat")

  m <- smt_model(s)
  expect_true("a" %in% names(m))
  expect_false("f" %in% names(m))
})

test_that("the API rejects things that are not solvers, and bad arguments", {
  expect_error(smt_check(list()), "must come from smt_solver")
  expect_error(smt_solver(42), "single string")
  expect_error(smt_solver("QF_BV"), "unsupported logic")

  s <- smt_solver()
  expect_error(smt_assert(s, c("a", "b")), "single string")
})

test_that("a released solver reports itself and refuses work", {
  s <- smt_solver("QF_UF")
  expect_output(print(s), "QF_UF")

  smt_release(s)
  expect_output(print(s), "released")
  expect_error(smt_check(s), "released")
})

test_that("the model reader survives a collection at every allocation", {
  skip_on_cran()  # gctorture makes this slow

  # Regression test for an unprotected-SEXP bug: model values were collected
  # in a std::vector<SEXP>, which R's garbage collector cannot see, so each
  # one was unprotected from the moment it was stored until the list was
  # built. Sixty declarations put hundreds of allocations in that window.
  #
  # Verified to fail before the fix: under gctorture the list came back
  # holding a CHARSXP -- "cannot have attributes on a CHARSXP" -- which is
  # collected memory handed back as a value. Three declarations were not
  # enough to show it, which is what makes this class of bug dangerous.
  decls <- paste0("(declare-const v", 1:60, " Int)", collapse = " ")
  asserts <- paste0("(assert (= v", 1:60, " ", 1:60, "))", collapse = " ")

  s <- smt_solver("QF_LIA")
  smt_assert(s, paste(decls, asserts))
  expect_identical(smt_check(s), "sat")

  gctorture2(1)
  on.exit(gctorture2(0), add = TRUE)
  m <- smt_model(s)
  gctorture2(0)

  expect_identical(names(m), paste0("v", 1:60))
  expect_equal(unname(vapply(m, as.numeric, numeric(1))), as.numeric(1:60))
})

test_that("output from reporting commands reaches the user", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const x Int) (assert (= x 42))")
  expect_identical(smt_check(s), "sat")

  # (get-model), (get-value), (get-info) and (echo) exist to print. Capturing
  # their output to decide about errors and then dropping it made them run and
  # report nothing.
  shown <- capture.output(smt_assert(s, "(get-model)"))
  expect_true(any(grepl("define-fun", shown, fixed = TRUE)))
  expect_true(any(grepl("42", shown, fixed = TRUE)))

  echoed <- capture.output(smt_assert(s, '(echo "hello")'))
  expect_true(any(grepl("hello", echoed, fixed = TRUE)))
})

test_that("a successful command is not mistaken for an error by its own output", {
  s <- smt_solver("QF_LIA")

  # The error oracle used to be a search for "(error" in the transcript, so
  # echoing that text turned a successful command into a failure. The solver
  # now reports errors itself, through notify_formatted().
  expect_silent(out <- capture.output(smt_assert(s, '(echo "(error not really)")')))
  expect_true(any(grepl("(error not really)", out, fixed = TRUE)))

  # And a real error is still an error.
  expect_error(smt_assert(s, "(assert (> nope 1))"), "Unknown symbol")
})

test_that("printing a solver does not solve it", {
  s <- smt_solver("QF_UF")
  # 10 pigeons into 9 holes: ~18s to decide. Printing must not wait for that.
  # The pigeonhole builder is an internal helper taking the handle itself,
  # not the S3 wrapper smt_solver() returns.
  solver_assert_pigeonhole(s$ptr, 9)

  elapsed <- system.time(invisible(capture.output(print(s))))[["elapsed"]]
  expect_lt(elapsed, 2)
})
