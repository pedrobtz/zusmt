// Temporary probe: does R_ToplevelExec(R_CheckUserInterrupt) consume the
// pending interrupt? Everything downstream of that question is different
// depending on the answer, and it is not something to reason about.
#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include <csignal>

namespace {
void check_inner(void *) { R_CheckUserInterrupt(); }
bool poll() { return R_ToplevelExec(check_inner, nullptr) == FALSE; }
}

// Raise SIGINT, then poll twice. If the first poll sees it and the second does
// not, the poll consumed it.
extern "C" SEXP C_probe_poll_twice(void) {
    std::raise(SIGINT);
    int const first = poll() ? 1 : 0;
    int const second = poll() ? 1 : 0;
    SEXP out = PROTECT(Rf_allocVector(INTSXP, 2));
    INTEGER(out)[0] = first;
    INTEGER(out)[1] = second;
    UNPROTECT(1);
    return out;
}

// Raise SIGINT, poll once (as the search loop does), then do what
// C_solver_check does at the end. If this returns, the interrupt was NOT
// delivered to R and the user gets a value instead of an interrupted call.
extern "C" SEXP C_probe_poll_then_check(void) {
    std::raise(SIGINT);
    int const saw = poll() ? 1 : 0;
    R_CheckUserInterrupt();          // the line under suspicion
    SEXP out = PROTECT(Rf_allocVector(INTSXP, 1));
    INTEGER(out)[0] = saw;
    UNPROTECT(1);
    return out;                      // reaching here means: not delivered
}
