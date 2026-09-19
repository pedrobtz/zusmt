#' Create a solver
#'
#' Creates a solver for one of the SMT-LIB logics, ready to be given assertions
#' with [smt_assert()]. The solver holds state: assertions accumulate until the
#' handle is released or garbage collected.
#'
#' @param logic An SMT-LIB logic name; [smt_logics()] returns the ones this
#'   package supports. They cover uninterpreted functions, linear integer and
#'   real arithmetic, their combinations, difference logic and arrays.
#' @param unsat_cores Whether to record enough of the search to report an
#'   unsat core with [smt_unsat_core()]. Costs time and memory on every solve,
#'   so it is off by default.
#' @param interpolants Whether to enable Craig interpolation, after which
#'   `(get-interpolants ...)` can be sent with [smt_assert()]. There is no
#'   dedicated R function for it yet; the output is printed rather than
#'   returned.
#' @return A solver handle, to be passed to the other `smt_*()` functions.
#'
#'   `unsat_cores` and `interpolants` are arguments here, rather than options
#'   to set later with [smt_assert()], because the solver decides whether to
#'   record a proof when it is built. Setting them afterwards cannot work, and
#'   the solver rejects the attempt.
#' @seealso [smt_assert()], [smt_check()], [smt_model()], [smt_unsat_core()]
#' @export
#' @examples
#' s <- smt_solver("QF_LIA")
#' smt_assert(s, "(declare-const x Int) (assert (> x 3))")
#' smt_check(s)
smt_solver <- function(logic = "QF_UF", unsat_cores = FALSE,
                       interpolants = FALSE) {
  if (!is.character(logic) || length(logic) != 1L) {
    stop("`logic` must be a single string", call. = FALSE)
  }
  structure(
    list(
      ptr = .Call(C_solver_new, logic, unsat_cores, interpolants),
      logic = logic
    ),
    class = "zusmt_solver"
  )
}

#' The unsat core of an unsatisfiable problem
#'
#' The subset of the assertions that is already unsatisfiable on its own --
#' the solver's explanation of why the answer was `"unsat"`.
#'
#' Requires a solver created with `smt_solver(unsat_cores = TRUE)`: whether a
#' proof is recorded is fixed when the solver is built, so it cannot be turned
#' on after the fact.
#'
#' @param solver A solver from [smt_solver()], created with
#'   `unsat_cores = TRUE`, on which [smt_check()] has returned `"unsat"`.
#' @param named_only Whether to report only assertions named with
#'   `(! ... :named n)`, which is what SMT-LIB means by an unsat core and what
#'   the `(get-unsat-core)` command returns. `FALSE`, the default, reports the
#'   core's assertions whether or not they were named -- an SMT-LIB-conformant
#'   core of a script that names nothing is empty, which is rarely the answer
#'   an R caller wants.
#' @return A character vector of assertions in the solver's own printed form,
#'   or of names when `named_only = TRUE`. If any assertion in the core was
#'   named, the names come back as the vector's `names()`.
#' @seealso [smt_solver()], [smt_check()]
#' @export
#' @examples
#' s <- smt_solver("QF_LIA", unsat_cores = TRUE)
#' smt_assert(s, "
#'   (declare-const x Int)
#'   (assert (! (> x 5) :named lower))
#'   (assert (! (< x 3) :named upper))
#'   (assert (! (= x 4) :named unrelated))
#' ")
#' smt_check(s)
#' smt_unsat_core(s)
#' smt_unsat_core(s, named_only = TRUE)
smt_unsat_core <- function(solver, named_only = FALSE) {
  check_solver(solver)
  .Call(C_solver_unsat_core, solver$ptr, named_only)
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
#' @return The solver, invisibly, so calls can be chained. Anything the script
#'   prints — the output of `get-model`, `get-value`, `get-info` or `echo`, for
#'   instance — is written to the console.
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

#' Print a solver
#'
#' @param x A solver from [smt_solver()].
#' @param ... Ignored, for compatibility with [print()].
#' @return `x`, invisibly. Called for the side effect of printing the solver's
#'   logic and whether it has been released.
#' @export
#' @examples
#' print(smt_solver("QF_LIA"))
print.zusmt_solver <- function(x, ...) {
  # A pointer check, not a solve. This used to call solver_check() to find out
  # whether the handle was still usable, which ran a full satisfiability check
  # to answer it -- more than a second on a modest problem, and unbounded on a
  # hard one, just to print one line.
  live <- solver_is_live(x$ptr)

  cat("<zusmt solver: ", x$logic, if (!live) " (released)" else "", ">\n", sep = "")
  invisible(x)
}

check_solver <- function(solver) {
  if (!inherits(solver, "zusmt_solver")) {
    stop("`solver` must come from smt_solver()", call. = FALSE)
  }
  invisible(TRUE)
}
