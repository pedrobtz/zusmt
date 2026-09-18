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
    # sys.call(), not sys.call(-1): the condition should name the function that
    # signalled it, the way stop() and warning() do. sys.call(-1) would name
    # whatever called solver_check(), attributing the interrupt to the user's
    # own wrapper.
    cond <- structure(
      class = c("interrupt", "condition"),
      list(message = "solve interrupted", call = sys.call())
    )
    signalCondition(cond)

    # Nothing handled it -- signalCondition() returns rather than unwinding --
    # so raise an error, because the one thing this must not do is hand back a
    # value. Note the divergence from a real Ctrl-C, which is deliberate: this
    # fallback is an error, so tryCatch(error = ) will catch what began as an
    # interrupt. Matching R exactly would mean setting R's internal pending
    # flag (Rinterface.h on Unix, UserBreak on Windows), which is not worth
    # reaching into for this.
    stop("solve interrupted")
  }

  result
}

solver_release <- function(solver) {
  invisible(.Call(C_solver_release, solver))
}

solver_is_live <- function(solver) {
  .Call(C_solver_is_live, solver)
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
