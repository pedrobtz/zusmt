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
  .Call(C_solver_check, solver)
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
