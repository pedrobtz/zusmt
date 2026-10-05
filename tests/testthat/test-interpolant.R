# A and B conflict only through y: A forces y >= 6, B forces y < 3. x belongs
# to A alone and z to B alone, so a correct interpolant can mention only y.
DECLS <- "(declare-const x Int) (declare-const y Int) (declare-const z Int)"
A_BODY <- "(and (> x 5) (= y x))"
B_BODY <- "(and (< z 3) (= z y))"

conflicting <- function() {
  s <- smt_solver("QF_LIA", interpolants = TRUE)
  smt_assert(s, sprintf("%s (assert (! %s :named A)) (assert (! %s :named B))",
                        DECLS, A_BODY, B_BODY))
  expect_identical(smt_check(s), "unsat")
  s
}

# Ask the solver whether a formula holds, rather than inspecting text.
unsat_with <- function(...) {
  v <- smt_solver("QF_LIA")
  smt_assert(v, paste(DECLS, paste0("(assert ", c(...), ")", collapse = " ")))
  smt_check(v)
}

test_that("the result is a Craig interpolant, by its definition", {
  itp <- smt_interpolant(conflicting(), "A")
  expect_length(itp, 1L)

  # The three defining properties, each checked with the solver rather than
  # by reading the string. A test that only matched printed output would pass
  # for any formula that happened to look plausible.

  # 1. A entails I -- so A together with the negation of I is unsatisfiable.
  expect_identical(unsat_with(A_BODY, sprintf("(not %s)", itp)), "unsat")

  # 2. I contradicts B.
  expect_identical(unsat_with(itp, B_BODY), "unsat")

  # 3. I is over the shared vocabulary: y appears, the private symbols do not.
  expect_match(itp, "y")
  expect_false(grepl("x", itp))
  expect_false(grepl("z", itp))
})

test_that("the interpolant for the other side is also one", {
  # Interpolation is not symmetric -- swapping the groups gives a different
  # formula -- but each must satisfy the definition against its own split.
  itp <- smt_interpolant(conflicting(), "B")

  expect_identical(unsat_with(B_BODY, sprintf("(not %s)", itp)), "unsat")
  expect_identical(unsat_with(itp, A_BODY), "unsat")
  expect_false(grepl("x", itp))
  expect_false(grepl("z", itp))
})

test_that("a contradiction closed before the search still interpolates", {
  # Raised in review of #13: s_False does not by itself mean a proof was
  # recorded. When the simplifier closes the formula before the SAT search
  # runs, the status is unsat with no search behind it -- and the
  # preconditions InterpolationContext states are asserts, compiled out under
  # NDEBUG, which is exactly how the #11 segfault reached users.
  #
  # Checked rather than assumed. Every degenerate shape returns a correct
  # interpolant: `false` follows from a contradictory A and is inconsistent
  # with anything, so it is the right answer, not a placeholder.
  degenerate <- list(
    c(decls = "", a = "false"),
    c(decls = "", a = "(= 1 2)"),
    c(decls = "(declare-const p Bool)", a = "(and p (not p))"),
    c(decls = "(declare-const w Int)", a = "(and (> w 5) (< w 3))")
  )

  for (case in degenerate) {
    s <- smt_solver("QF_LIA", interpolants = TRUE, unsat_cores = TRUE)
    smt_assert(s, sprintf("%s (assert (! %s :named A)) (assert (! true :named B))",
                          case[["decls"]], case[["a"]]))
    expect_identical(smt_check(s), "unsat")

    itp <- smt_interpolant(s, "A")
    expect_length(itp, 1L)
    expect_identical(itp, "false", info = case[["a"]])

    # The unsat core reaches the proof by the same route, so it gets the same
    # question asked of it.
    expect_no_error(smt_unsat_core(s))
  }
})

test_that("interpolation must be enabled when the solver is built", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, sprintf("%s (assert (! %s :named A)) (assert (! %s :named B))",
                        DECLS, A_BODY, B_BODY))
  expect_identical(smt_check(s), "unsat")
  expect_error(smt_interpolant(s, "A"), "interpolants = TRUE")
})

test_that("an interpolant needs an unsatisfiable check first", {
  s <- smt_solver("QF_LIA", interpolants = TRUE)
  smt_assert(s, sprintf("%s (assert (! %s :named A))", DECLS, A_BODY))
  expect_identical(smt_check(s), "sat")
  expect_error(smt_interpolant(s, "A"), "only available after an unsatisfiable check")
})

test_that("a name that is not a top-level assertion is refused, not ignored", {
  # Silently dropping an unknown name would compute an interpolant for a
  # smaller A than the caller asked for -- a wrong answer rather than an
  # error, and one that still looks like a valid interpolant.
  expect_error(smt_interpolant(conflicting(), "nope"), "no assertion is named 'nope'")
  expect_error(smt_interpolant(conflicting(), c("A", "nope")), "no assertion is named 'nope'")
})

test_that("`a` must name assertions", {
  s <- conflicting()
  expect_error(smt_interpolant(s, 42), "character vector")
  expect_error(smt_interpolant(s, character(0)), "character vector")
  expect_error(smt_interpolant(s, NA_character_), "character vector")
})

test_that("an interpolant is refused once the assertions have changed", {
  s <- conflicting()
  smt_assert(s, "(assert (> x 100))")
  expect_error(smt_interpolant(s, "A"), class = "zusmt_stale_result")
})

test_that("interpolants are written in SMT-LIB numerals", {
  # pp() wrote -64/5, which no parser but OpenSMT's accepts (#29).
  s <- smt_solver("QF_LRA", interpolants = TRUE)
  smt_assert(s, "(declare-const x Real) (declare-const y Real)
    (assert (! (and (> (* 5 x) 16) (= y (* 4 x))) :named A))
    (assert (! (< y 3) :named B))")
  expect_identical(smt_check(s), "unsat")
  itp <- smt_interpolant(s, "A")

  # No bare negative numeral and no bare p/q.
  expect_false(any(grepl("(^|[ (])-[0-9]", itp)))
  expect_false(any(grepl("[0-9]/[0-9]", itp)))

  # And it is still an interpolant: A implies it, and it contradicts B.
  v <- smt_solver("QF_LRA")
  smt_assert(v, sprintf("(declare-const x Real) (declare-const y Real)
    (assert (and (> (* 5 x) 16) (= y (* 4 x)))) (assert (not %s))", itp))
  expect_identical(smt_check(v), "unsat")
  w <- smt_solver("QF_LRA")
  smt_assert(w, sprintf("(declare-const y Real) (assert %s) (assert (< y 3))", itp))
  expect_identical(smt_check(w), "unsat")
})

test_that("a logic the solver cannot interpolate gives one classed error", {
  s <- smt_solver("QF_AX", interpolants = TRUE)
  smt_assert(s, "(declare-sort E 0) (declare-const a (Array E E))
    (declare-const i E) (declare-const e E) (declare-const f E)
    (assert (! (= (select (store a i e) i) f) :named A))
    (assert (! (not (= e f)) :named B))")
  expect_identical(smt_check(s), "unsat")
  expect_error(smt_interpolant(s, "A"), "interpolation is not supported for this QF_AX",
               class = "zusmt_unsupported_input")
})

test_that("interpolation can stop QF_LIA from terminating, and a timeout bounds it", {
  # 2x = y = 2z + 1 is unsat by parity, and decided instantly -- unless
  # interpolation is on, which disables the cuts that decide it (#29). This
  # pins the documented behaviour; if a future OpenSMT returns "unsat" here,
  # that is an improvement and the documentation can say so.
  skip_on_cran()
  body <- "(declare-const x Int) (declare-const y Int) (declare-const z Int)
    (assert (! (and (= (* 2 x) y) (= y (+ (* 2 z) 1))) :named A))
    (assert (! (> z 0) :named B))"

  plain <- smt_solver("QF_LIA")
  smt_assert(plain, body)
  expect_identical(smt_check(plain), "unsat")

  s <- smt_solver("QF_LIA", interpolants = TRUE)
  smt_assert(s, body)
  expect_warning(result <- smt_check(s, timeout = 2), class = "zusmt_timeout")
  expect_identical(result, "unknown")
})
