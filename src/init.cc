#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Visibility.h>

extern "C" SEXP C_toolchain_report(void);
extern "C" SEXP C_smoke_solve(SEXP contradiction);

static R_CallMethodDef const CallEntries[] = {
    {"C_toolchain_report", (DL_FUNC) &C_toolchain_report, 0},
    {"C_smoke_solve", (DL_FUNC) &C_smoke_solve, 1},
    {NULL, NULL, 0}
};

extern "C" attribute_visible void R_init_zusmt(DllInfo * dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
    R_forceSymbols(dll, TRUE);
}
