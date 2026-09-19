# SMT-LIB symbols are not ASCII-only: |naive| with a diaeresis is a legal
# quoted symbol. Written with \u escapes so this file stays ASCII -- the
# characters are what the test needs, not the bytes in the source.
NAIVE <- "naïve"   # naïve
CAFE <- "café"     # café

test_that("non-ASCII symbols survive the round trip into the model", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, sprintf("(declare-const |%s| Int) (assert (= |%s| 7))", NAIVE, NAIVE))
  expect_identical(smt_check(s), "sat")

  m <- smt_model(s)
  expect_identical(names(m), NAIVE)
  # The point of the test: not merely that the bytes came back, but that R was
  # told what they are. Marked native, these would be reinterpreted through
  # the session's locale -- correct on a UTF-8 session and wrong elsewhere,
  # which is the kind of accident that only shows up on someone else's machine.
  expect_identical(Encoding(names(m)), "UTF-8")
  expect_true(validUTF8(names(m)))
})

test_that("non-ASCII names and terms come back declared from the unsat core", {
  s <- smt_solver("QF_LIA", unsat_cores = TRUE)
  smt_assert(s, sprintf(
    "(declare-const |%s| Int)
     (assert (! (> |%s| 5) :named |%s|))
     (assert (! (< |%s| 3) :named plain))",
    NAIVE, NAIVE, CAFE, NAIVE))
  expect_identical(smt_check(s), "unsat")

  core <- smt_unsat_core(s)
  expect_true(all(validUTF8(core)))
  expect_true(all(Encoding(core) == "UTF-8"))
  expect_true(CAFE %in% names(core))

  named <- smt_unsat_core(s, named_only = TRUE)
  expect_true(CAFE %in% named)
  expect_identical(Encoding(named[named == CAFE]), "UTF-8")
})

test_that("pure ASCII is left unmarked", {
  # R stores ASCII without an encoding mark whatever mkCharCE is told, so this
  # pins that the change costs nothing in the ordinary case rather than
  # marking every string in the package UTF-8.
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const x Int) (assert (= x 1))")
  expect_identical(smt_check(s), "sat")
  expect_identical(Encoding(names(smt_model(s))), "unknown")
})
