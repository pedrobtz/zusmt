#' Create a solver
#'
#' Creates a solver for one of the SMT-LIB logics, ready to be given assertions
#' with [smt_assert()]. The solver holds state: assertions accumulate until the
#' handle is released or garbage collected.
#'
#' @param logic An SMT-LIB logic name; [smt_logics()] returns the ones this
#'   package supports. They cover uninterpreted functions, linear integer and
#'   real arithmetic, their combinations, difference logic and arrays.
#' @return A solver handle, to be passed to the other `smt_*()` functions.
#' @seealso [smt_assert()], [smt_check()], [smt_model()]
#' @export
#' @examples
#' s <- smt_solver("QF_LIA")
#' smt_assert(s, "(declare-const x Int) (assert (> x 3))")
#' smt_check(s)
smt_solver <- function(logic = "QF_UF") {
  if (!is.character(logic) || length(logic) != 1L) {
    stop("`logic` must be a single string", call. = FALSE)
  }
  structure(
    list(ptr = .Call(C_solver_new, logic), logic = logic),
    class = "zusmt_solver"
  )
}

#' The logics this package supports
#'
#' The supported set is defined once, in the C++ layer, and read from there —
#' so this function, the solver's own check and the package's tests cannot
#' disagree about it.
#'
#' @return A character vector of SMT-LIB logic names accepted by
#'   [smt_solver()].
#' @export
#' @examples
#' smt_logics()
smt_logics <- function() {
  .Call(C_supported_logics)
}

#' Send SMT-LIB input to a solver
#'
#' Runs SMT-LIB2 text through the bundled solver. Despite the name, any
#' SMT-LIB2 commands are accepted, not only `assert` — declarations, `push` and
#' `pop`, options, and so on — because the package is a front end to the
#' solver's own language rather than a curated subset of it.
#'
#' Use [smt_check()] rather than a `(check-sat)` command: it returns the result
#' to R instead of printing it.
#'
#' @param solver A solver from [smt_solver()].
#' @param text A single string of SMT-LIB2 input. Newlines are fine, and
#'   several commands may appear in one call.
#' @return The solver, invisibly, so calls can be chained.
#' @export
#' @examples
#' s <- smt_solver("QF_LRA")
#' smt_assert(s, "
#'   (declare-const x Real)
#'   (declare-const y Real)
#'   (assert (> x y))
#'   (assert (> y 0.0))
#' ")
#' smt_check(s)
smt_assert <- function(solver, text) {
  check_solver(solver)
  if (!is.character(text) || length(text) != 1L) {
    stop("`text` must be a single string", call. = FALSE)
  }
  .Call(C_solver_run, solver$ptr, text)
  invisible(solver)
}

#' Check satisfiability
#'
#' @param solver A solver from [smt_solver()].
#' @return `"sat"`, `"unsat"` or `"unknown"`.
#'
#'   A long search can be interrupted. Because the poll that notices a pending
#'   interrupt also consumes it, the interrupt is re-signalled from R as a
#'   condition inheriting from `interrupt`, so `tryCatch(interrupt = )` works;
#'   with no handler it becomes an error rather than returning a value.
#' @export
#' @examples
#' s <- smt_solver("QF_LIA")
#' smt_assert(s, "(declare-const x Int) (assert (> x 0)) (assert (< x 0))")
#' smt_check(s)
smt_check <- function(solver) {
  check_solver(solver)
  solver_check(solver$ptr)
}

#' The model of a satisfiable problem
#'
#' @param solver A solver from [smt_solver()], on which [smt_check()] has just
#'   returned `"sat"`.
#' @return A named list, one element per 0-ary declaration. Booleans come back
#'   as logicals and numbers as doubles carrying an `"exact"` attribute with
#'   the solver's exact rational, since an SMT rational need not be
#'   representable as a double. Values of other sorts come back as the solver's
#'   own printed form.
#'
#'   Functions of arity greater than zero are omitted: they have no single
#'   value to report.
#' @export
#' @examples
#' s <- smt_solver("QF_LIA")
#' smt_assert(s, "(declare-const x Int) (assert (= x 42))")
#' smt_check(s)
#' smt_model(s)
smt_model <- function(solver) {
  check_solver(solver)
  .Call(C_solver_model, solver$ptr)
}

#' Release a solver
#'
#' Frees the solver's memory without waiting for garbage collection. Using the
#' solver afterwards is an error rather than a crash.
#'
#' @param solver A solver from [smt_solver()].
#' @return `NULL`, invisibly.
#' @export
#' @examples
#' s <- smt_solver()
#' smt_release(s)
smt_release <- function(solver) {
  check_solver(solver)
  solver_release(solver$ptr)
  invisible(NULL)
}

#' @export
print.zusmt_solver <- function(x, ...) {
  released <- tryCatch({
    solver_check(x$ptr)
    FALSE
  }, error = function(e) grepl("released", conditionMessage(e), fixed = TRUE))

  cat("<zusmt solver: ", x$logic, if (released) " (released)" else "", ">\n", sep = "")
  invisible(x)
}

check_solver <- function(solver) {
  if (!inherits(solver, "zusmt_solver")) {
    stop("`solver` must come from smt_solver()", call. = FALSE)
  }
  invisible(TRUE)
}
