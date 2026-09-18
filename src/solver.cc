// Solver handles: the external-pointer half of the boundary.
//
// A handle owns the three objects that make up a solve, in an order that
// matters: MainSolver holds references to the Logic and the SMTConfig, so it
// has to be destroyed first. Declaring them in this order and letting the
// implicit destructor run in reverse is what guarantees that -- there is no
// hand-written destructor to get wrong.

#include "boundary.h"
#include "r_compat.h"

#include <api/MainSolver.h>
#include <logics/ArithLogic.h>
#include <logics/Logic.h>
#include <logics/LogicFactory.h>
#include <options/SMTConfig.h>

#include <memory>
#include <stdexcept>
#include <string>

namespace {

struct SolverHandle {
    // Declaration order is destruction order, reversed: solver, then config,
    // then logic. Do not reorder.
    std::unique_ptr<opensmt::Logic> logic;
    std::unique_ptr<opensmt::SMTConfig> config;
    std::unique_ptr<opensmt::MainSolver> solver;
};

// The tag distinguishes our external pointers from anyone else's. Without it,
// passing some other package's pointer here would reinterpret_cast its
// address and crash.
SEXP solver_tag() {
    static SEXP tag = Rf_install("zusmt_solver");
    return tag;
}

void finalize_solver(SEXP xp) {
    auto * handle = static_cast<SolverHandle *>(R_ExternalPtrAddr(xp));
    if (handle == nullptr) return;  // already released, or never set
    delete handle;
    // Clearing means a double finalize -- gc() after an explicit release --
    // finds nullptr and returns above rather than freeing twice.
    R_ClearExternalPtr(xp);
}

SolverHandle * handle_from(SEXP xp) {
    if (TYPEOF(xp) != EXTPTRSXP || R_ExternalPtrTag(xp) != solver_tag()) {
        throw std::invalid_argument("not a zusmt solver handle");
    }
    auto * handle = static_cast<SolverHandle *>(R_ExternalPtrAddr(xp));
    if (handle == nullptr) {
        throw std::runtime_error("this solver handle has been released");
    }
    return handle;
}

opensmt::Logic_t logic_from_name(std::string const & name) {
    if (name == "QF_UF") return opensmt::Logic_t::QF_UF;
    if (name == "QF_LRA") return opensmt::Logic_t::QF_LRA;
    if (name == "QF_LIA") return opensmt::Logic_t::QF_LIA;
    throw std::invalid_argument("unsupported logic: " + name);
}

}  // namespace

extern "C" SEXP C_solver_new(SEXP logic_name) {
    return zusmt::with_firewall([&]() -> SEXP {
        if (TYPEOF(logic_name) != STRSXP || Rf_length(logic_name) != 1) {
            throw std::invalid_argument("logic must be a single string");
        }
        std::string const name(CHAR(STRING_ELT(logic_name, 0)));
        opensmt::Logic_t const which = logic_from_name(name);

        auto handle = std::make_unique<SolverHandle>();
        if (which == opensmt::Logic_t::QF_UF) {
            handle->logic = std::make_unique<opensmt::Logic>(which);
        } else {
            handle->logic = std::make_unique<opensmt::ArithLogic>(which);
        }
        handle->config = std::make_unique<opensmt::SMTConfig>();
        handle->solver = std::make_unique<opensmt::MainSolver>(*handle->logic, *handle->config, "zusmt");

        // Allocate the R object only once the C++ side is fully built: if
        // this allocation triggers a gc that errors, there is no half-built
        // handle to leak, and the unique_ptr still owns everything.
        SEXP xp = PROTECT(R_MakeExternalPtr(handle.get(), solver_tag(), R_NilValue));
        R_RegisterCFinalizerEx(xp, finalize_solver, TRUE);
        handle.release();  // ownership now belongs to the finalizer
        UNPROTECT(1);
        return xp;
    });
}

// Asserts a boolean variable, or its negation. Deliberately minimal: this is
// the boundary, not the API -- Stage 6 designs term building. It exists so the
// handle can be exercised across several calls.
extern "C" SEXP C_solver_assert_var(SEXP xp, SEXP name, SEXP negated) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);
        if (TYPEOF(name) != STRSXP || Rf_length(name) != 1) {
            throw std::invalid_argument("name must be a single string");
        }
        opensmt::PTRef var = handle->logic->mkBoolVar(CHAR(STRING_ELT(name, 0)));
        opensmt::PTRef term = (Rf_asLogical(negated) == TRUE) ? handle->logic->mkNot(var) : var;
        handle->solver->insertFormula(term);
        return R_NilValue;
    });
}

extern "C" SEXP C_solver_check(SEXP xp) {
    SEXP answer = PROTECT(zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);

        zusmt::clear_interrupt_request();
        opensmt::sstat const status = handle->solver->check();

        char const * result = "error";
        if (zusmt::interrupt_was_requested()) {
            // The search stopped early because a poll saw a pending
            // interrupt. Say so, and let the caller below raise it.
            result = "interrupted";
        } else if (status == opensmt::s_True) {
            result = "sat";
        } else if (status == opensmt::s_False) {
            result = "unsat";
        } else if (status == opensmt::s_Undef) {
            result = "unknown";
        }
        return Rf_mkString(result);
    }));

    // No R_CheckUserInterrupt() here, deliberately. It would be a no-op: the
    // poll that noticed the interrupt is what consumed R's pending flag, so
    // by this point there is nothing left for it to raise. Measured, not
    // assumed -- tests/testthat/test-interrupt.R pins it.
    //
    // Delivery happens in solver_check() in R, which signals an interrupt
    // condition of its own. This function's contract is to *report*
    // "interrupted", not to raise it.
    UNPROTECT(1);
    return answer;
}

// Releases the solver early rather than waiting for gc. Idempotent: the
// finalizer nulls the pointer, and handle_from() rejects a null one, so a
// second release is an R error rather than a double free.
extern "C" SEXP C_solver_release(SEXP xp) {
    return zusmt::with_firewall([&]() -> SEXP {
        (void) handle_from(xp);  // validates tag and liveness
        finalize_solver(xp);
        return R_NilValue;
    });
}

// Pigeonhole: n+1 pigeons into n holes, which is unsatisfiable and takes
// resolution exponential time. Here so the tests have a solve long enough to
// interrupt, and so term building beyond a single variable is exercised
// before Stage 6 designs it properly.
extern "C" SEXP C_solver_assert_pigeonhole(SEXP xp, SEXP holes_) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);
        int const holes = Rf_asInteger(holes_);
        if (holes < 1 || holes > 20) {
            throw std::invalid_argument("holes must be between 1 and 20");
        }
        int const pigeons = holes + 1;
        opensmt::Logic & logic = *handle->logic;

        auto var = [&](int pigeon, int hole) {
            std::string const name = "p" + std::to_string(pigeon) + "_h" + std::to_string(hole);
            return logic.mkBoolVar(name.c_str());
        };

        // Every pigeon is in some hole.
        for (int p = 0; p < pigeons; ++p) {
            opensmt::vec<opensmt::PTRef> someHole;
            for (int h = 0; h < holes; ++h) someHole.push(var(p, h));
            handle->solver->insertFormula(logic.mkOr(std::move(someHole)));
        }
        // No hole holds two pigeons.
        for (int h = 0; h < holes; ++h) {
            for (int p1 = 0; p1 < pigeons; ++p1) {
                for (int p2 = p1 + 1; p2 < pigeons; ++p2) {
                    handle->solver->insertFormula(
                        logic.mkOr(logic.mkNot(var(p1, h)), logic.mkNot(var(p2, h))));
                }
            }
        }
        return R_NilValue;
    });
}

// Test hook: see zusmt::arm_test_interrupt.
extern "C" SEXP C_arm_test_interrupt(SEXP polls) {
    return zusmt::with_firewall([&]() -> SEXP {
        zusmt::arm_test_interrupt(Rf_asInteger(polls));
        return R_NilValue;
    });
}

// Exists only so the tests can prove an upstream C++ exception arrives in R as
// an ordinary condition rather than taking the process down.
extern "C" SEXP C_throw_from_cpp(void) {
    return zusmt::with_firewall([]() -> SEXP {
        throw std::runtime_error("deliberate exception from C++");
    });
}
