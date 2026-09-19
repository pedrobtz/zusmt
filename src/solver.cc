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
#include <common/TermNames.h>
#include <unsatcores/UnsatCore.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

// Interpret keeps the logic, the solver and the list of user declarations as
// protected members, and exposes only getMainSolver(). Subclassing is how a
// consumer is meant to reach the rest, and it beats patching the vendored
// header: nothing here has to be re-applied at the next version bump.
class RInterpret final : public opensmt::Interpret {
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

    zusmt::clear_error();
    CaptureScope capture;
    int const parse_status = handle.interp->interpFile(buffer.data());
    std::string output = capture.take();

    // A parse failure and a semantic complaint arrive differently: the first
    // as a non-zero return, the second through notify_formatted(error = true),
    // which a patch rule has record itself. Both have to become exceptions, or
    // a mistyped script would look like it had been accepted.
    //
    // Asking the solver rather than searching its output for "(error" matters
    // in both directions: (echo "(error ...)") succeeds and would otherwise be
    // rejected, and a change to upstream's error format would otherwise pass
    // errors through as success.
    if (parse_status != 0) {
        throw std::runtime_error(output.empty() ? "could not parse SMT-LIB input" : output);
    }
    if (zusmt::error_was_reported()) {
        throw std::runtime_error(output.empty() ? "the solver reported an error" : output);
    }

    // Whatever the script printed is the whole point of commands like
    // (get-model), (get-value ...), (get-info ...) and (echo ...). Capturing
    // it to decide about errors and then dropping it made those commands run
    // and report nothing.
    if (!output.empty()) {
        Rprintf("%s", output.c_str());
    }
}

// The attribute carrying a model value's exact rational.
SEXP exact_tag() {
    static SEXP tag = Rf_install("exact");
    return tag;
}

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

// A TRUE/FALSE argument from R onto one of SMTConfig's boolean options.
// setOption() reports refusal through an out-parameter and a bool rather than
// by throwing, so the result has to be checked: ignoring it is how an option
// silently fails to take effect.
void set_flag_option(opensmt::SMTConfig & config, char const * option, SEXP flag,
                     char const * argument) {
    if (TYPEOF(flag) != LGLSXP || Rf_length(flag) != 1 ||
        LOGICAL(flag)[0] == NA_LOGICAL) {
        throw std::invalid_argument(std::string("`") + argument +
                                    "` must be TRUE or FALSE");
    }
    if (LOGICAL(flag)[0] != TRUE) return;

    char const * message = nullptr;
    if (!config.setOption(option, opensmt::SMTOption(1), message)) {
        throw std::runtime_error(std::string("could not enable ") + argument + ": " +
                                 (message != nullptr ? message : "unknown reason"));
    }
}

}  // namespace

extern "C" SEXP C_solver_new(SEXP logic_name, SEXP unsat_cores, SEXP interpolants) {
    return zusmt::with_firewall([&]() -> SEXP {
        if (TYPEOF(logic_name) != STRSXP || Rf_length(logic_name) != 1) {
            throw std::invalid_argument("logic must be a single string");
        }
        std::string const name(CHAR(STRING_ELT(logic_name, 0)));
        check_logic_supported(name);

        auto handle = std::make_unique<SolverHandle>();
        handle->config = std::make_unique<opensmt::SMTConfig>();

        // Before set-logic, and it has to be: the SAT solver allocates its
        // ResolutionProof in its constructor, so an option that decides
        // whether there is a proof at all is only meaningful while no solver
        // exists. SMTConfig agrees -- it refuses these three once it has been
        // used for initialization -- which is why they are arguments to
        // smt_solver() rather than something to (set-option) later.
        set_flag_option(*handle->config, opensmt::SMTConfig::o_produce_unsat_cores,
                        unsat_cores, "unsat_cores");
        set_flag_option(*handle->config, opensmt::SMTConfig::o_produce_inter,
                        interpolants, "interpolants");

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
    return zusmt::with_firewall([&]() -> SEXP {
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
    });
}

// No R_CheckUserInterrupt() anywhere above, deliberately. It would be a no-op:
// the poll that noticed the interrupt is what consumed R's pending flag, so by
// the time check() returns there is nothing left for it to raise. Measured,
// not assumed -- tests/testthat/test-interrupt.R pins it.
//
// Delivery happens in solver_check() in R, which signals an interrupt
// condition of its own. C_solver_check()'s contract is to *report*
// "interrupted", not to raise it.

// Is this handle still usable? A tag and pointer check, nothing more: the R
// print method needs to know whether a solver has been released, and using
// C_solver_check() for that ran a full satisfiability check to answer it.
extern "C" SEXP C_solver_is_live(SEXP xp) {
    return zusmt::with_firewall([&]() -> SEXP {
        bool const live = TYPEOF(xp) == EXTPTRSXP &&
                          R_ExternalPtrTag(xp) == solver_tag() &&
                          R_ExternalPtrAddr(xp) != nullptr;
        return Rf_ScalarLogical(live ? TRUE : FALSE);
    });
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

// The unsat core: which assertions are already unsatisfiable together.
//
// Read from the solver rather than parsed out of (get-unsat-core) output,
// the same choice the model reader makes and for the same reason -- the
// printed form is upstream's to change.
//
// `named_only` picks between two genuinely different answers. SMT-LIB defines
// an unsat core over *named* assertions only, so a script that names nothing
// has an empty core by definition; that is what the SMT-LIB command reports
// and what named_only = TRUE reproduces. It is also useless to a caller who
// did not use (! ... :named n), which in R is the common case, so the default
// reports the core's actual terms instead. Upstream already supports both --
// UnsatCoreBuilder branches on :print-cores-full -- so this selects that
// option rather than reimplementing either behaviour.
extern "C" SEXP C_solver_unsat_core(SEXP xp, SEXP named_only) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);

        if (TYPEOF(named_only) != LGLSXP || Rf_length(named_only) != 1 ||
            LOGICAL(named_only)[0] == NA_LOGICAL) {
            throw std::invalid_argument("`named_only` must be TRUE or FALSE");
        }
        bool const want_full = LOGICAL(named_only)[0] != TRUE;

        if (!handle->config->produce_unsat_cores()) {
            throw std::runtime_error(
                "unsat cores were not enabled; create the solver with "
                "smt_solver(unsat_cores = TRUE)");
        }

        opensmt::MainSolver & solver = handle->interp->getMainSolver();
        if (solver.getStatus() != opensmt::s_False) {
            // getUnsatCore() checks this too and throws ApiException, which
            // the firewall would convert anyway. Checking first is what lets
            // the message name the R function the caller actually used.
            throw std::runtime_error(
                "an unsat core is only available after an unsatisfiable check");
        }

        // :print-cores-full is read while the core is built, not while the
        // solver is, so it can be chosen per call -- but it also governs the
        // (get-unsat-core) command, which must keep SMT-LIB's meaning. Hence
        // set, build, restore, with the restore on a destructor so an
        // exception out of build() cannot leave it flipped.
        struct FullCoreOption {
            opensmt::SMTConfig & config;
            bool const previous;

            FullCoreOption(opensmt::SMTConfig & config_, bool wanted)
                : config{config_}, previous{config_.print_cores_full()} {
                set(wanted);
            }
            ~FullCoreOption() { set(previous); }

            void set(bool value) {
                char const * message = nullptr;
                config.setOption(opensmt::SMTConfig::o_print_cores_full,
                                 opensmt::SMTOption(value ? 1 : 0), message);
            }
        } const full_core{*handle->config, want_full};

        std::unique_ptr<opensmt::UnsatCore> const core = solver.getUnsatCore();
        opensmt::vec<opensmt::PTRef> const & terms = core->getTerms();
        opensmt::Logic & logic = handle->interp->theLogic();
        // as_const: the non-const getTermNames() is deprecated in favour of
        // the mutating helpers, and only the read-only overload is wanted
        // here. Upstream's own getInterpolants() reaches for it the same way.
        opensmt::TermNames const & term_names = std::as_const(solver).getTermNames();

        SEXP out = PROTECT(Rf_allocVector(STRSXP, terms.size()));
        SEXP names = R_NilValue;
        int named = 0;

        for (int i = 0; i < terms.size(); ++i) {
            std::string const * const name = term_names.tryGetNameForTerm(terms[i]);
            if (want_full) {
                SET_STRING_ELT(out, i, Rf_mkChar(logic.printTerm(terms[i]).c_str()));
            } else {
                // Without :print-cores-full every term here is a named one,
                // so this lookup cannot fail -- but a null would be a silent
                // empty string, so say so instead.
                if (name == nullptr) {
                    UNPROTECT(1);
                    throw std::runtime_error("the solver reported an unnamed term in a named core");
                }
                SET_STRING_ELT(out, i, Rf_mkChar(name->c_str()));
            }
            if (name != nullptr) ++named;
        }

        // Names as an R attribute, so a caller who used (! ... :named n) gets
        // them back without giving up the terms. Omitted entirely when
        // nothing was named, rather than filling a vector with "".
        if (want_full && named > 0) {
            names = PROTECT(Rf_allocVector(STRSXP, terms.size()));
            for (int i = 0; i < terms.size(); ++i) {
                std::string const * const name = term_names.tryGetNameForTerm(terms[i]);
                SET_STRING_ELT(names, i,
                               name != nullptr ? Rf_mkChar(name->c_str()) : Rf_mkChar(""));
            }
            Rf_setAttrib(out, R_NamesSymbol, names);
            UNPROTECT(1);
        }

        UNPROTECT(1);
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
