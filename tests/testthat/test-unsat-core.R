unsat_solver <- function(...) {
  s <- smt_solver("QF_LIA", unsat_cores = TRUE, ...)
  smt_assert(s, "
    (declare-const x Int)
    (assert (! (> x 5) :named lower))
    (assert (! (< x 3) :named upper))
    (assert (! (= x 4) :named unrelated))
  ")
  expect_identical(smt_check(s), "unsat")
  s
}

test_that("enabling unsat cores after the solver exists is refused, not fatal", {
  # This is the regression. (set-option :produce-unsat-cores true) here used
  # to be accepted, because SMTConfig did not list it among the options that
  # cannot move after initialization -- and the SAT solver had already been
  # built with no ResolutionProof to go with the flag it had just flipped.
  # (get-unsat-core) then bound a reference to a null pointer and walked it,
  # segfaulting the R session. A patch rule adds the option to that list, so
  # the attempt now fails the way :produce-interpolants already did.
  #
  # The crash itself cannot be asserted from here: it killed the process
  # rather than raising a condition. What this pins is the boundary that
  # makes it unreachable.
  s <- smt_solver("QF_LIA")
  expect_error(
    smt_assert(s, "(set-option :produce-unsat-cores true)"),
    "cannot be changed at this point"
  )
})

test_that("the core is the assertions that conflict, and not the others", {
  core <- smt_unsat_core(unsat_solver())

  # x > 5 and x < 3 conflict on their own; x = 4 is satisfiable with either.
  # A core that returned all three would be trivially correct and useless.
  expect_named(core, c("lower", "upper"), ignore.order = TRUE)
  expect_length(core, 2L)
})

test_that("named_only picks between SMT-LIB's answer and a usable one", {
  s <- unsat_solver()

  expect_setequal(smt_unsat_core(s, named_only = TRUE), c("lower", "upper"))

  # Nothing named: SMT-LIB defines the core over named assertions, so the
  # conformant answer is empty. The default reports the terms instead, which
  # is the whole reason it is the default.
  anon <- smt_solver("QF_LIA", unsat_cores = TRUE)
  smt_assert(anon, "(declare-const x Int) (assert (> x 5)) (assert (< x 3))")
  expect_identical(smt_check(anon), "unsat")

  expect_length(smt_unsat_core(anon, named_only = TRUE), 0L)
  expect_length(smt_unsat_core(anon), 2L)
  expect_null(names(smt_unsat_core(anon)))
})

test_that("asking for a core the solver cannot give is an error", {
  off <- smt_solver("QF_LIA")
  smt_assert(off, "(declare-const x Int) (assert (> x 5)) (assert (< x 3))")
  expect_identical(smt_check(off), "unsat")
  expect_error(smt_unsat_core(off), "unsat_cores = TRUE")

  sat <- smt_solver("QF_LIA", unsat_cores = TRUE)
  smt_assert(sat, "(declare-const x Int) (assert (> x 5))")
  expect_identical(smt_check(sat), "sat")
  expect_error(smt_unsat_core(sat), "only available after an unsatisfiable check")
})

test_that("smt_unsat_core() leaves the SMT-LIB command's meaning alone", {
  # The default flips :print-cores-full to get terms rather than names. That
  # option also governs (get-unsat-core), which has to keep reporting names,
  # so it is restored afterwards. Without the restore this prints terms.
  s <- unsat_solver()
  invisible(smt_unsat_core(s))

  expect_output(smt_assert(s, "(get-unsat-core)"), "lower")
  expect_output(smt_assert(s, "(get-unsat-core)"), "upper")
  expect_failure(expect_output(smt_assert(s, "(get-unsat-core)"), "<="))
})

test_that("the option arguments reject anything that is not TRUE or FALSE", {
  expect_error(smt_solver("QF_LIA", unsat_cores = "yes"), "must be TRUE or FALSE")
  expect_error(smt_solver("QF_LIA", unsat_cores = NA), "must be TRUE or FALSE")
  expect_error(smt_solver("QF_LIA", interpolants = 1), "must be TRUE or FALSE")
  expect_error(smt_unsat_core(unsat_solver(), named_only = "yes"), "must be TRUE or FALSE")
})

test_that("interpolants are reachable when enabled at construction", {
  # Craig interpolation was unreachable for the same structural reason the
  # crash was: the option is fixed when the solver is built, and set-logic
  # ran inside smt_solver() before a caller could say anything.
  s <- smt_solver("QF_LIA", interpolants = TRUE)
  smt_assert(s, "
    (declare-const x Int)
    (assert (! (> x 5) :named A))
    (assert (! (< x 3) :named B))
  ")
  expect_identical(smt_check(s), "unsat")
  expect_output(smt_assert(s, "(get-interpolants A B)"), "x")

  off <- smt_solver("QF_LIA")
  expect_error(
    smt_assert(off, "(set-option :produce-interpolants true)"),
    "cannot be changed at this point"
  )
})
