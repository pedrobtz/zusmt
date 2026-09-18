#' Report the toolchain the package was compiled with
#'
#' `zusmt` bundles the OpenSMT solver, which requires a C++20 compiler, the
#' GMP library including its C++ bindings, and threads. This function reports
#' what the installed package was actually built against, which is the
#' information needed to diagnose an installation that compiled but misbehaves.
#'
#' @return A named list with the compiler's `__cplusplus` value, the compiler
#'   identification, the GMP runtime version, the result of a rational
#'   arithmetic round trip (`"1/2"`), and whether the computation ran on a
#'   separate thread.
#' @export
#' @examples
#' smt_toolchain()
smt_toolchain <- function() {
  .Call(C_toolchain_report)
}
