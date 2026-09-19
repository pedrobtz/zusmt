# Stage 5 scaffolding: the boundary, not the API. Stage 6 designs what users
# see; these exist so the tests can exercise handle lifetime, error conversion
# and interruption.

solver_new <- function(logic = "QF_UF", unsat_cores = FALSE,
                       interpolants = FALSE) {
  .Call(C_solver_new, logic, unsat_cores, interpolants)
}

solver_assert_var <- function(solver, name, negated = FALSE) {
  invisible(.Call(C_solver_assert_var, solver, name, negated))
}

solver_check <- function(solver, timeout = Inf) {
  result <- .Call(C_solver_check, solver, as.double(timeout))

  if (identical(result, "timeout")) {
    # A timeout is an "unknown" -- the solver stopped without deciding, which
    # is exactly what SMT-LIB means by it -- so that is what comes back, and
    # code testing for "unknown" keeps working. But returning it silently
    # would make a bounded solve indistinguishable from a solver that genuinely
    # gave up, so it is also a warning: visible by default, catchable, and it
    # does not change the value the way an attribute would.
    warning(structure(
      class = c("zusmt_timeout", "warning", "condition"),
      list(
        message = sprintf("solve timed out after %g seconds; result is \"unknown\"", timeout),
        call = sys.call()
      )
    ))
    return("unknown")
  }

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
