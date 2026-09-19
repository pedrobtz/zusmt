# Pigeonhole is the standard hard-but-small instance: n+1 pigeons into n
# holes is unsatisfiable and takes resolution exponential time, so a solver
# with no bound sits on it. 12-into-11 takes about two minutes here
# unbounded, which is what makes the bounded case meaningful -- a test that
# timed out a problem the solver would have finished anyway proves nothing.
hard_solve <- function() {
  s <- solver_new("QF_UF")
  solver_assert_pigeonhole(s, 11)
  s
}

test_that("a timeout stops a solve that would otherwise run for minutes", {
  elapsed <- system.time(
    result <- suppressWarnings(solver_check(hard_solve(), timeout = 0.5))
  )[["elapsed"]]

  expect_identical(result, "unknown")
  # Generous by two orders of magnitude against the unbounded runtime: this
  # asserts the bound was honoured at all, not how punctual it is. CI runners
  # are slow and the deadline is only checked in the SAT loop.
  expect_lt(elapsed, 30)
})

test_that("a timeout warns, so it is not mistaken for the solver giving up", {
  # "unknown" is the honest result -- nothing was decided -- but a silent one
  # would be indistinguishable from a solver that gave up on its own. The
  # warning carries the difference, and a class so it can be caught.
  expect_warning(
    solver_check(hard_solve(), timeout = 0.5),
    class = "zusmt_timeout"
  )

  cond <- tryCatch(solver_check(hard_solve(), timeout = 0.5), warning = function(w) w)
  expect_s3_class(cond, "zusmt_timeout")
  expect_match(conditionMessage(cond), "timed out")
})

test_that("a decided answer is reported however late it arrives", {
  # The deadline can expire between the solver's last okContinue() check and
  # it finishing, so "the clock has passed" does not mean "the search was
  # stopped". Reporting a timeout on the clock alone discards a correct sat or
  # unsat that the solver had already reached.
  #
  # Reproduced with a problem decided without ever entering the search loop --
  # a propositional contradiction, settled in preprocessing -- under a
  # deadline that has already expired. okContinue() never gets to stop
  # anything, so the status is decided while the clock says otherwise.
  #
  # An arithmetic contradiction does not work here: that one does reach the
  # search, whose first okContinue() sees the expired deadline and stops it,
  # and "unknown" is then the correct answer.
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(assert false)")

  expect_silent(result <- smt_check(s, timeout = 1e-9))
  expect_identical(result, "unsat")
})

test_that("a solve that finishes in time is untouched", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const x Int) (assert (> x 3)) (assert (< x 7))")

  expect_silent(result <- smt_check(s, timeout = 60))
  expect_identical(result, "sat")

  # No attribute, no class, no change of any kind to the returned value --
  # the warning is the whole mechanism, so existing code comparing against
  # "sat" keeps working.
  expect_identical(smt_check(s, timeout = 60), smt_check(s))
})

test_that("an expired deadline does not bound the next solve", {
  # The deadline is process-wide state, cleared by a destructor on the way out
  # of C_solver_check(). Without that, one timed-out solve would silently make
  # every later one return "unknown" immediately.
  invisible(suppressWarnings(solver_check(hard_solve(), timeout = 0.5)))

  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const y Int) (assert (= y 1))")
  expect_silent(result <- smt_check(s))
  expect_identical(result, "sat")
  # ignore_attr: model values carry the solver's exact rational.
  expect_equal(smt_model(s)$y, 1, ignore_attr = TRUE)
})

test_that("timeout rejects values that cannot mean a deadline", {
  s <- smt_solver("QF_LIA")
  smt_assert(s, "(declare-const x Int) (assert (> x 0))")

  expect_error(smt_check(s, timeout = 0), "positive number")
  expect_error(smt_check(s, timeout = -1), "positive number")
  expect_error(smt_check(s, timeout = NA_real_), "positive number")
  # NaN is the one that needs saying: `seconds <= 0` is false for it, so an
  # unchecked NaN arms a deadline that can never expire -- a timeout argument
  # that silently means "no timeout".
  expect_error(smt_check(s, timeout = NaN), "positive number")

  expect_error(smt_check(s, timeout = "5"), "single number")
  expect_error(smt_check(s, timeout = c(1, 2)), "single number")

  expect_identical(smt_check(s, timeout = Inf), "sat")
})
