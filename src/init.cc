#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include <R_ext/Rdynload.h>
#include <R_ext/Visibility.h>

extern "C" SEXP C_toolchain_report(void);
extern "C" SEXP C_solver_new(SEXP logic_name, SEXP unsat_cores, SEXP interpolants);
extern "C" SEXP C_solver_assert_var(SEXP xp, SEXP name, SEXP negated);
extern "C" SEXP C_solver_check(SEXP xp);
extern "C" SEXP C_supported_logics(void);
extern "C" SEXP C_solver_run(SEXP xp, SEXP text);
extern "C" SEXP C_solver_model(SEXP xp);
extern "C" SEXP C_solver_unsat_core(SEXP xp, SEXP named_only);
extern "C" SEXP C_solver_is_live(SEXP xp);
extern "C" SEXP C_solver_release(SEXP xp);
extern "C" SEXP C_solver_assert_pigeonhole(SEXP xp, SEXP holes);
extern "C" SEXP C_arm_test_interrupt(SEXP polls);
extern "C" SEXP C_probe_poll_twice(void);
extern "C" SEXP C_probe_poll_then_check(void);
extern "C" SEXP C_throw_from_cpp(void);

static R_CallMethodDef const CallEntries[] = {
    {"C_toolchain_report", (DL_FUNC) &C_toolchain_report, 0},
    {"C_solver_new", (DL_FUNC) &C_solver_new, 3},
    {"C_solver_assert_var", (DL_FUNC) &C_solver_assert_var, 3},
    {"C_solver_check", (DL_FUNC) &C_solver_check, 1},
    {"C_supported_logics", (DL_FUNC) &C_supported_logics, 0},
    {"C_solver_run", (DL_FUNC) &C_solver_run, 2},
    {"C_solver_model", (DL_FUNC) &C_solver_model, 1},
    {"C_solver_unsat_core", (DL_FUNC) &C_solver_unsat_core, 2},
    {"C_solver_is_live", (DL_FUNC) &C_solver_is_live, 1},
    {"C_solver_release", (DL_FUNC) &C_solver_release, 1},
    {"C_solver_assert_pigeonhole", (DL_FUNC) &C_solver_assert_pigeonhole, 2},
    {"C_arm_test_interrupt", (DL_FUNC) &C_arm_test_interrupt, 1},
    {"C_probe_poll_twice", (DL_FUNC) &C_probe_poll_twice, 0},
    {"C_probe_poll_then_check", (DL_FUNC) &C_probe_poll_then_check, 0},
    {"C_throw_from_cpp", (DL_FUNC) &C_throw_from_cpp, 0},
    {NULL, NULL, 0}
};

extern "C" attribute_visible void R_init_zusmt(DllInfo * dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
    R_forceSymbols(dll, TRUE);
}
