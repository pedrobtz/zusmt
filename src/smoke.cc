// Stage 3 of roadmap.md: proof that the vendored solver links and runs.
//
// Not the package API -- Stage 6 designs that. This asserts two tiny QF_UF
// problems with known answers through MainSolver and hands the result back to
// R, which is the smallest thing that can distinguish "87 files compiled and
// linked" from "the solver actually works in this process".

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <api/MainSolver.h>
#include <logics/Logic.h>
#include <logics/LogicFactory.h>
#include <options/SMTConfig.h>

#include <cstdio>
#include <string>

namespace {

// "a AND NOT a" is unsat; "a" alone is sat. Between them they exercise term
// construction, the assertion stack, and both answers from the SAT core.
char const * solve_one(bool contradiction) {
    opensmt::Logic logic{opensmt::Logic_t::QF_UF};
    opensmt::SMTConfig config;
    opensmt::MainSolver solver(logic, config, "zusmt smoke");

    opensmt::PTRef a = logic.mkBoolVar("a");
    opensmt::PTRef fla = contradiction ? logic.mkAnd(a, logic.mkNot(a)) : a;
    solver.insertFormula(fla);

    opensmt::sstat r = solver.check();
    if (r == opensmt::s_True) return "sat";
    if (r == opensmt::s_False) return "unsat";
    if (r == opensmt::s_Undef) return "unknown";
    return "error";
}

}  // namespace

// Same exception firewall as toolchain.cc: the message is copied into a plain
// buffer and every C++ object destroyed before Rf_error() longjmps past it.
extern "C" SEXP C_smoke_solve(SEXP contradiction) {
    char errbuf[256] = "";
    char answer[16] = "";

    {
        try {
            bool const contra = (Rf_asLogical(contradiction) == TRUE);
            std::snprintf(answer, sizeof answer, "%s", solve_one(contra));
        } catch (std::exception const & e) {
            std::snprintf(errbuf, sizeof errbuf, "%s", e.what());
        } catch (...) {
            std::snprintf(errbuf, sizeof errbuf, "unknown C++ exception");
        }
    }

    if (errbuf[0] != '\0') {
        Rf_error("zusmt smoke solve failed: %s", errbuf);
    }
    return Rf_mkString(answer);
}
