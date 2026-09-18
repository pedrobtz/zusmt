test_that("an interrupted solve raises an interrupt condition, not a value", {
  s <- solver_new()
  solver_assert_pigeonhole(s, 9)
  arm_test_interrupt(3)

  caught <- tryCatch(solver_check(s), interrupt = function(e) "caught")
  expect_identical(caught, "caught")
})

test_that("an interrupted solve never returns a value to its caller", {
  s <- solver_new()
  solver_assert_pigeonhole(s, 9)
  arm_test_interrupt(3)

  # A real Ctrl-C never hands the caller a value, and neither may this: the
  # C level says "interrupted", the R level must turn that into a condition.
  #
  # Both handlers are established here, inside the test, so they are found
  # before testthat's own interrupt handler -- which is why this does not
  # abort the test the way an unhandled signal would. That path (no handler
  # anywhere, so signalCondition returns and stop() runs) cannot be exercised
  # under testthat, because testthat always handles interrupts.
  outcome <- tryCatch(
    solver_check(s),
    interrupt = function(e) "signalled-interrupt",
    error = function(e) "errored"
  )

  expect_true(outcome %in% c("signalled-interrupt", "errored"))
  expect_false(identical(outcome, "interrupted"))
})

test_that("an uninterrupted solve returns normally", {
  s <- solver_new()
  solver_assert_var(s, "a")
  expect_identical(solver_check(s), "sat")
})

# This pins the R behaviour the design depends on. If a future R stops clearing
# the pending flag inside R_ToplevelExec, solver_check() would signal an
# interrupt that R then delivers a second time, and we would want to know.
test_that("polling for an interrupt consumes it (the reason for the R-level signal)", {
  skip_on_cran()
  skip_on_os("windows")  # raise(SIGINT) does not reach R's handler the same way

  seen <- .Call(C_probe_poll_twice)
  expect_identical(seen[1], 1L)  # the first poll sees the pending interrupt
  expect_identical(seen[2], 0L)  # and has consumed it

  # Therefore a later R_CheckUserInterrupt() cannot deliver it: this returns.
  expect_type(.Call(C_probe_poll_then_check), "integer")
})
