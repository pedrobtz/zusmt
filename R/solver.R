# Stage 5 scaffolding: the boundary, not the API. Stage 6 designs what users
# see; these exist so the tests can exercise handle lifetime, error conversion
# and interruption.

solver_new <- function(logic = "QF_UF") {
  .Call(C_solver_new, logic)
}

solver_assert_var <- function(solver, name, negated = FALSE) {
  invisible(.Call(C_solver_assert_var, solver, name, negated))
}

solver_check <- function(solver) {
  result <- .Call(C_solver_check, solver)

  if (identical(result, "interrupted")) {
    # The search stopped because a poll saw a pending interrupt -- and that
    # poll is what consumed it. Verified rather than assumed: raising SIGINT
    # and polling twice reports pending once, and a subsequent
    # R_CheckUserInterrupt() returns normally (tests/testthat/test-interrupt.R).
    #
    # So the interrupt has to be re-signalled here, at R level, or the user
    # would press Ctrl-C and get a return value -- something a real interrupt
    # never does.
    cond <- structure(
      class = c("interrupt", "condition"),
      list(message = "solve interrupted", call = sys.call(-1))
    )
    signalCondition(cond)

    # Unhandled: signalCondition() returns, so turn it into an error rather
    # than let the call succeed with a value.
    stop("solve interrupted", call. = FALSE)
  }

  result
}

solver_release <- function(solver) {
  invisible(.Call(C_solver_release, solver))
}

throw_from_cpp <- function() {
  .Call(C_throw_from_cpp)
}

solver_assert_pigeonhole <- function(solver, holes) {
  invisible(.Call(C_solver_assert_pigeonhole, solver, as.integer(holes)))
}

arm_test_interrupt <- function(polls) {
  invisible(.Call(C_arm_test_interrupt, as.integer(polls)))
}
