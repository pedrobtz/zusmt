// Solver handles: the external-pointer half of the boundary.
//
// A handle owns the three objects that make up a solve, in an order that
// matters: MainSolver holds references to the Logic and the SMTConfig, so it
// has to be destroyed first. Declaring them in this order and letting the
// implicit destructor run in reverse is what guarantees that -- there is no
// hand-written destructor to get wrong.

#include "boundary.h"
#include "r_compat.h"

#include <api/Interpret.h>
#include <api/MainSolver.h>
#include <logics/ArithLogic.h>
#include <logics/Logic.h>
#include <logics/LogicFactory.h>
#include <options/SMTConfig.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

// Interpret keeps the logic, the solver and the list of user declarations as
// protected members, and exposes only getMainSolver(). Subclassing is how a
// consumer is meant to reach the rest, and it beats patching the vendored
// header: nothing here has to be re-applied at the next version bump.
class RInterpret : public opensmt::Interpret {
public:
    using opensmt::Interpret::Interpret;

    opensmt::Logic & theLogic() { return *logic; }
    opensmt::vec<opensmt::SymRef> const & declarations() const { return user_declarations; }
};

struct SolverHandle {
    // Declaration order is destruction order, reversed: the interpreter (and
    // the solver it owns) goes first, then the config it refers to. Do not
    // reorder.
    std::unique_ptr<opensmt::SMTConfig> config;
    std::unique_ptr<RInterpret> interp;
};

// Capture is a process-wide flag, and only end_capture() clears it. If the
// solver throws while it is on, every later write to the shim streams would
// disappear into the buffer instead of reaching the console -- and a check or
// a model call in that window establishes no capture of its own, so its
// diagnostics would vanish silently. RAII rather than a careful ordering of
// statements: with_firewall() exists precisely because this code assumes
// upstream can throw.
class CaptureScope {
public:
    CaptureScope() { zusmt::begin_capture(); }
    CaptureScope(CaptureScope const &) = delete;
    CaptureScope & operator=(CaptureScope const &) = delete;
    ~CaptureScope() {
        if (!taken_) (void) zusmt::end_capture();
    }
    std::string take() {
        taken_ = true;
        return zusmt::end_capture();
    }

private:
    bool taken_ = false;
};

// interpFile() takes a mutable char* because flex scans the buffer in place.
void run_script(SolverHandle & handle, std::string const & script) {
    std::vector<char> buffer(script.begin(), script.end());
    buffer.push_back('\0');

    CaptureScope capture;
    int const parse_status = handle.interp->interpFile(buffer.data());
    std::string output = capture.take();

    // A parse failure and a semantic complaint arrive differently: the first
    // as a non-zero return, the second only as printed (error "...") text.
    // Both have to become exceptions, or a mistyped script would look like it
    // had been accepted.
    if (parse_status != 0) {
        throw std::runtime_error(output.empty() ? "could not parse SMT-LIB input" : output);
    }
    if (output.find("(error") != std::string::npos) {
        throw std::runtime_error(output);
    }
}

// The tag distinguishes our external pointers from anyone else's. Without it,
// passing some other package's pointer here would reinterpret_cast its
// address and crash.
SEXP exact_tag() {
    static SEXP tag = Rf_install("exact");
    return tag;
}

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

// The single place the supported set is written down.
//
// Deliberately a short list rather than everything upstream accepts: these
// are the logics the package tests, and widening it is a decision with test
// obligations attached rather than a typo fix. R's smt_logics() reads this
// array through C_supported_logics(), and the documentation, the corpus
// header check and the per-logic tests all derive from that -- so there is no
// second copy for someone to keep in agreement by hand.
char const * const kSupportedLogics[] = {
    "QF_UF", "QF_LIA", "QF_LRA", "QF_UFLIA", "QF_UFLRA", "QF_IDL", "QF_RDL", "QF_AX"
};

void check_logic_supported(std::string const & name) {
    for (char const * supported : kSupportedLogics) {
        if (name == supported) return;
    }
    throw std::invalid_argument("unsupported logic: " + name);
}

}  // namespace

extern "C" SEXP C_solver_new(SEXP logic_name) {
    return zusmt::with_firewall([&]() -> SEXP {
        if (TYPEOF(logic_name) != STRSXP || Rf_length(logic_name) != 1) {
            throw std::invalid_argument("logic must be a single string");
        }
        std::string const name(CHAR(STRING_ELT(logic_name, 0)));
        check_logic_supported(name);

        auto handle = std::make_unique<SolverHandle>();
        handle->config = std::make_unique<opensmt::SMTConfig>();
        handle->interp = std::make_unique<RInterpret>(*handle->config);

        // The interpreter builds its logic and solver when it sees set-logic,
        // so the handle is not usable until this runs. Doing it here means a
        // bad logic name fails at solver_new() rather than at the first
        // assert.
        run_script(*handle, "(set-logic " + name + ")");

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
        opensmt::PTRef var = handle->interp->theLogic().mkBoolVar(CHAR(STRING_ELT(name, 0)));
        opensmt::Logic & logic = handle->interp->theLogic();
        opensmt::PTRef term = (Rf_asLogical(negated) == TRUE) ? logic.mkNot(var) : var;
        handle->interp->getMainSolver().insertFormula(term);
        return R_NilValue;
    });
}

extern "C" SEXP C_solver_check(SEXP xp) {
    SEXP answer = PROTECT(zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);

        zusmt::clear_interrupt_request();
        opensmt::sstat const status = handle->interp->getMainSolver().check();

        char const * result = "error";
        if (zusmt::interrupt_was_requested()) {
            // The search stopped early because a poll saw a pending
            // interrupt. Report it; solver_check() in R is what raises it,
            // because the poll that detected it also consumed R's pending
            // flag, leaving nothing for this side to raise.
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

// The supported logics, so R never has to restate them.
extern "C" SEXP C_supported_logics(void) {
    return zusmt::with_firewall([]() -> SEXP {
        R_xlen_t const n = static_cast<R_xlen_t>(sizeof kSupportedLogics / sizeof kSupportedLogics[0]);
        SEXP out = PROTECT(Rf_allocVector(STRSXP, n));
        for (R_xlen_t i = 0; i < n; ++i) {
            SET_STRING_ELT(out, i, Rf_mkChar(kSupportedLogics[i]));
        }
        UNPROTECT(1);
        return out;
    });
}

// Runs SMT-LIB2 text through the bundled interpreter. Any commands are
// allowed, not only assertions: the package's job here is to be a faithful
// front end to the solver's own language rather than a curated subset.
extern "C" SEXP C_solver_run(SEXP xp, SEXP text) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);
        if (TYPEOF(text) != STRSXP || Rf_length(text) != 1) {
            throw std::invalid_argument("SMT-LIB input must be a single string");
        }
        run_script(*handle, CHAR(STRING_ELT(text, 0)));
        return R_NilValue;
    });
}

// The model, as a named list. Only 0-ary declarations are reported: an
// uninterpreted function of arity > 0 has no single value to put in a list,
// and inventing one would be worse than omitting it.
extern "C" SEXP C_solver_model(SEXP xp) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);
        opensmt::MainSolver & solver = handle->interp->getMainSolver();

        if (solver.getStatus() != opensmt::s_True) {
            throw std::runtime_error("a model is only available after a satisfiable check");
        }

        opensmt::Logic & logic = handle->interp->theLogic();
        auto * arith = dynamic_cast<opensmt::ArithLogic *>(&logic);
        std::unique_ptr<opensmt::Model> model = solver.getModel();
        opensmt::vec<opensmt::SymRef> const & declarations = handle->interp->declarations();

        // Count first, so the result list can be allocated at its final size
        // and every value stored straight into it. Collecting SEXPs in a
        // std::vector on the way would leave them unprotected: R's collector
        // cannot see C++ containers, and this loop allocates repeatedly.
        R_xlen_t reported = 0;
        for (opensmt::SymRef sym : declarations) {
            if (logic.getSym(sym).nargs() == 0) ++reported;
        }

        SEXP out = PROTECT(Rf_allocVector(VECSXP, reported));
        SEXP names = PROTECT(Rf_allocVector(STRSXP, reported));

        R_xlen_t at = 0;
        for (opensmt::SymRef sym : declarations) {
            if (logic.getSym(sym).nargs() != 0) continue;

            opensmt::PTRef const term = logic.mkUninterpFun(sym, {});
            opensmt::PTRef const value = model->evaluate(term);
            SET_STRING_ELT(names, at, Rf_mkChar(logic.getSymName(sym)));

            if (value == logic.getTerm_true()) {
                SET_VECTOR_ELT(out, at, Rf_ScalarLogical(TRUE));
            } else if (value == logic.getTerm_false()) {
                SET_VECTOR_ELT(out, at, Rf_ScalarLogical(FALSE));
            } else if (arith != nullptr && arith->isNumConst(value)) {
                // A double loses exactness, and SMT rationals routinely are
                // not representable in one. Report the double for arithmetic,
                // and the exact value as an attribute for anyone who needs it.
                opensmt::Number const & number = arith->getNumConst(value);
                SEXP num = PROTECT(Rf_ScalarReal(number.get_d()));
                // num is protected across this allocation, then handed to a
                // list that is itself protected.
                Rf_setAttrib(num, exact_tag(), Rf_mkString(number.get_str().c_str()));
                SET_VECTOR_ELT(out, at, num);
                UNPROTECT(1);
            } else {
                // Anything else -- an uninterpreted sort's value, say --
                // comes back as the solver's own printed form rather than
                // being coerced into an R type it does not fit.
                SET_VECTOR_ELT(out, at, Rf_mkString(logic.printTerm(value).c_str()));
            }
            ++at;
        }

        Rf_setAttrib(out, R_NamesSymbol, names);
        UNPROTECT(2);
        return out;
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
        opensmt::Logic & logic = handle->interp->theLogic();

        auto var = [&](int pigeon, int hole) {
            std::string const name = "p" + std::to_string(pigeon) + "_h" + std::to_string(hole);
            return logic.mkBoolVar(name.c_str());
        };

        // Every pigeon is in some hole.
        for (int p = 0; p < pigeons; ++p) {
            opensmt::vec<opensmt::PTRef> someHole;
            for (int h = 0; h < holes; ++h) someHole.push(var(p, h));
            handle->interp->getMainSolver().insertFormula(logic.mkOr(std::move(someHole)));
        }
        // No hole holds two pigeons.
        for (int h = 0; h < holes; ++h) {
            for (int p1 = 0; p1 < pigeons; ++p1) {
                for (int p2 = p1 + 1; p2 < pigeons; ++p2) {
                    handle->interp->getMainSolver().insertFormula(
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
