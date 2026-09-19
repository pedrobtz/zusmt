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
