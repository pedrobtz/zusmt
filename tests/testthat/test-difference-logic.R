# QF_IDL and QF_RDL are decided by a solver that reads every atom as a
# difference constraint and, before this was checked, misread anything else:
# a confident "sat" with a model violating the input, or "unsat" for a
# satisfiable problem (#29). Such input is now refused before it reaches the
# solver, with a class of its own.

expect_unsupported <- function(solver, text) {
  expect_error(smt_assert(solver, text), class = "zusmt_unsupported_input")
}

test_that("a sum of two variables is refused under QF_IDL, not misread", {
  s <- smt_solver("QF_IDL")
  smt_assert(s, "(declare-const x Int) (declare-const y Int) (declare-const z Int)")
  # Was "sat" with x = 1, y = 1, z = 0, which violates 1 + 1 < 0.
  expect_unsupported(s, "(assert (< (+ x y) z))")
  # The refused assertion was not added: the rest still solves.
  smt_assert(s, "(assert (> x 0)) (assert (> y 0)) (assert (< z 2))")
  expect_identical(smt_check(s), "sat")
})

test_that("a sum is refused under QF_RDL, not misread", {
  r <- smt_solver("QF_RDL")
  smt_assert(r, "(declare-const x Real) (declare-const y Real)")
  # Was "sat" with x = 3, y = 1 for x + y < 1 and x + y > 2.
  expect_unsupported(r, "(assert (< (+ x y) 1.0))")
  expect_unsupported(r, "(assert (> (+ x y) 2.0))")
})

test_that("coefficients other than one are refused, which was a wrong unsat", {
  # Satisfiable (x1 = -3, x2 = 3 per QF_LIA and brute force) and was reported
  # "unsat" under QF_IDL.
  u <- smt_solver("QF_IDL")
  smt_assert(u, "(declare-const x1 Int) (declare-const x2 Int)
    (assert (and (>= x1 (- 3)) (<= x1 3))) (assert (and (>= x2 (- 3)) (<= x2 3)))")
  expect_unsupported(u, "(assert (>= (+ (* (- 2) x1) (* 3 x2)) (- 4)))")
  expect_unsupported(u, "(assert (< (* (- 2) x2) x1))")
  # A single scaled variable is fine: it normalises to x2 > 2, a bound.
  smt_assert(u, "(assert (< (* (- 2) x2) (- 4)))")
  expect_unsupported(u, "(assert (distinct (+ x1 x2) 0))")

  l <- smt_solver("QF_LIA")
  smt_assert(l, "(declare-const x1 Int) (declare-const x2 Int)
    (assert (and (>= x1 (- 3)) (<= x1 3))) (assert (and (>= x2 (- 3)) (<= x2 3)))
    (assert (>= (+ (* (- 2) x1) (* 3 x2)) (- 4)))
    (assert (and (distinct (+ (* (- 1) x1) x2) (- 3))
                 (> (+ (* (- 2) x1) (* (- 2) x2)) (- 4))
                 (<= (+ (* (- 1) x1) (* (- 3) x2)) (- 3))))
    (assert (< (* (- 2) x2) (- 4)))")
  expect_identical(smt_check(l), "sat")
})

test_that("an arithmetic ite is checked as the equalities it becomes", {
  s <- smt_solver("QF_IDL")
  smt_assert(s, "(declare-const x Int) (declare-const y Int) (declare-const z Int)")
  expect_unsupported(s, "(assert (= x (ite (> y 0) (+ y z) z)))")
})

test_that("the error names the atom and the logic to use instead", {
  s <- smt_solver("QF_IDL")
  smt_assert(s, "(declare-const x Int) (declare-const y Int)")
  err <- tryCatch(smt_assert(s, "(assert (<= (+ x y) 3))"), error = identity)
  expect_s3_class(err, "zusmt_error")
  expect_match(conditionMessage(err), "QF_IDL accepts only difference constraints")
  expect_match(conditionMessage(err), "contains (<= ", fixed = TRUE)
  expect_match(conditionMessage(err), "QF_LIA")
})

test_that("every form of difference constraint is still accepted and solved", {
  s <- smt_solver("QF_IDL")
  smt_assert(s, "
    (declare-const x Int) (declare-const y Int) (declare-const z Int)
    (assert (<= (- x y) 3))
    (assert (>= (- 4) (- x y)))
    (assert (< x y))
    (assert (= x z))
    (assert (distinct x y))
    (assert (> x 0))
    (assert (not (= y 7)))
    (assert (= y (ite (> x 1) (+ z 4) (+ x 4))))
    (assert (<= (* 2 x) 8))
  ")
  expect_identical(smt_check(s), "sat")
  m <- vapply(smt_model(s), as.numeric, numeric(1))
  expect_lte(m[["x"]] - m[["y"]], -4)
  expect_lt(m[["x"]], m[["y"]])
  expect_equal(m[["x"]], m[["z"]])
  expect_gt(m[["x"]], 0)
  expect_lte(m[["x"]], 4)
  expect_equal(m[["y"]], if (m[["x"]] > 1) m[["z"]] + 4 else m[["x"]] + 4)
})

test_that("random problems agree with QF_LIA/QF_LRA, or are refused", {
  # The review's sweep in small: random linear atoms over a bounded domain,
  # half of them difference constraints. Every answer must match the general
  # solver, every model must satisfy the input, and only non-difference input
  # may be refused.
  set.seed(29)
  vars <- c("x1", "x2", "x3")
  num <- function(k, real) {
    s <- if (real) paste0(abs(k), ".0") else as.character(abs(k))
    if (k < 0) sprintf("(- %s)", s) else s
  }
  difference_atom <- function(real) {
    op <- sample(c("<", "<=", ">", ">=", "="), 1)
    v <- sample(vars, 2)
    c <- num(sample(-3:3, 1), real)
    switch(sample(4, 1),
      sprintf("(%s (- %s %s) %s)", op, v[1], v[2], c),
      sprintf("(%s %s %s)", op, v[1], v[2]),
      sprintf("(%s %s %s)", op, v[1], c),
      sprintf("(distinct %s %s)", v[1], v[2]))
  }
  linear_atom <- function(real) {
    v <- sample(vars, 2)
    k <- sample(c(-2, 2, 3), 1)
    sprintf("(%s (+ %s (* %s %s)) %s)", sample(c("<=", ">", "="), 1),
            v[1], num(k, real), v[2], num(sample(-3:3, 1), real))
  }

  for (i in 1:60) {
    real <- i %% 2 == 0
    general <- i %% 3 == 0
    sort <- if (real) "Real" else "Int"
    script <- paste(
      paste(sprintf("(declare-const %s %s)", vars, sort), collapse = " "),
      paste(sprintf("(assert (and (>= %1$s %2$s) (<= %1$s %3$s)))", vars,
                    num(-3, real), num(3, real)), collapse = " "),
      paste(sprintf("(assert %s)", replicate(4, if (general && runif(1) < 0.5)
        linear_atom(real) else difference_atom(real))), collapse = " ")
    )

    reference <- smt_solver(if (real) "QF_LRA" else "QF_LIA")
    smt_assert(reference, script)
    expected <- smt_check(reference)

    dl <- smt_solver(if (real) "QF_RDL" else "QF_IDL")
    refused <- tryCatch({
      smt_assert(dl, script)
      FALSE
    }, zusmt_unsupported_input = function(e) TRUE)
    if (refused) {
      expect_true(general, info = script)
      next
    }
    expect_identical(smt_check(dl), expected, info = script)

    if (expected == "sat") {
      m <- smt_model(dl)
      pinned <- smt_solver(if (real) "QF_LRA" else "QF_LIA")
      smt_assert(pinned, script)
      # From the exact rational, since a strict bound can put a real model
      # value between integers.
      literal <- function(v) {
        parts <- as.numeric(strsplit(attr(v, "exact"), "/", fixed = TRUE)[[1]])
        if (length(parts) == 1L) num(parts, real)
        else sprintf("(/ %s %s)", num(parts[1], TRUE), num(parts[2], TRUE))
      }
      smt_assert(pinned, paste(sprintf("(assert (= %s %s))", names(m),
        vapply(m, literal, "")), collapse = " "))
      expect_identical(smt_check(pinned), "sat", info = script)
    }
  }
})
