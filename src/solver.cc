// Solver handles: the external-pointer half of the boundary.
//
// A handle owns the three objects that make up a solve, in an order that
// matters: MainSolver holds references to the Logic and the SMTConfig, so it
// has to be destroyed first. Declaring them in this order and letting the
// implicit destructor run in reverse is what guarantees that -- there is no
// hand-written destructor to get wrong.

#include "boundary.h"
#include "difference_logic.h"
#include "r_compat.h"

#include <api/Interpret.h>
#include <api/MainSolver.h>
#include <api/smt2tokens.h>
#include <common/ApiException.h>
#include <logics/ArithLogic.h>
#include <logics/Logic.h>
#include <logics/LogicFactory.h>
#include <options/SMTConfig.h>
#include <common/Partitions.h>
#include <common/TermNames.h>
#include <proof/InterpolationContext.h>
#include <unsatcores/UnsatCore.h>

#include <memory>
#include <stdexcept>
#include <optional>
#include <string>
#include <unordered_set>
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

    // Set for QF_IDL and QF_RDL, whose solver reads only difference atoms.
    bool difference_logic = false;

    // One top-level command. interp() is upstream's, and runs everything but
    // an assert under a difference logic: that one is checked between being
    // parsed and reaching the solver, which interp() has no hook for. The
    // assert branch below is interp()'s own t_assert case plus that check.
    void command(opensmt::ASTNode & n) {
        if (!difference_logic || n.getToken().x != opensmt::tokens::t_assert || !isInitialized()) {
            interp(n);
            return;
        }
        opensmt::PTRef tr = opensmt::PTRef_Undef;
        try {
            opensmt::ASTNode const & asrt = **(n.children->begin());
            opensmt::LetRecords letRecords;
            tr = parseTerm(asrt, letRecords);
        } catch (opensmt::ApiException const & e) {
            notify_formatted(true, e.what());
            return;
        }
        if (tr == opensmt::PTRef_Undef) {
            notify_formatted(true, "assertion returns an unknown sort");
            return;
        }
        // Throws before anything is inserted, so a refused assertion leaves
        // the solver exactly as it was.
        check_difference_atoms(tr);
        assertions.push(tr);
        try {
            main_solver->insertFormula(tr);
            notify_success();
        } catch (opensmt::ApiException const & e) {
            notify_formatted(true, e.what());
        }
    }

private:
    // Every arithmetic atom in the assertion must be one the difference-logic
    // solver can read; see difference_logic.h. Besides inequalities and
    // equalities, two constructs become atoms only during preprocessing, and
    // are checked as the atoms they will become: (distinct a b ...) turns into
    // the pairwise (= a b), and an arithmetic (ite c t e) into a fresh
    // variable v with (= v t) and (= v e).
    void check_difference_atoms(opensmt::PTRef root) {
        auto & arith = dynamic_cast<opensmt::ArithLogic &>(*logic);
        std::unordered_set<uint32_t> seen;
        std::vector<opensmt::PTRef> pending{root};

        auto require = [&](opensmt::PTRef atom, bool ok) {
            if (ok) return;
            throw zusmt::condition_error(
                "zusmt_unsupported_input",
                std::string(arith.getName()) +
                    " accepts only difference constraints -- comparisons of the form "
                    "(op (- x y) c), (op x c) or (op x y) -- and this assertion contains " +
                    arith.printTerm(atom) +
                    ", which is not one. The assertion was not added. Use " +
                    (arith.hasReals() ? "QF_LRA" : "QF_LIA") +
                    " for general linear arithmetic.");
        };
        // An equality the preprocessor will build; trivially true or false
        // ones need no theory solver at all.
        auto require_equality = [&](opensmt::PTRef a, opensmt::PTRef b, opensmt::PTRef context) {
            opensmt::PTRef const eq = arith.mkEq(a, b);
            if (eq == arith.getTerm_true() || eq == arith.getTerm_false()) return;
            require(context, arith.isNumEq(eq) && zusmt::dl_equality(arith, eq, true));
        };

        while (!pending.empty()) {
            opensmt::PTRef const t = pending.back();
            pending.pop_back();
            if (!seen.insert(t.x).second) continue;

            // Copied out: mkEq() below may grow the term table, which
            // invalidates references into it.
            std::vector<opensmt::PTRef> args;
            {
                opensmt::Pterm const & term = arith.getPterm(t);
                for (int i = 0; i < term.size(); ++i) args.push_back(term[i]);
            }

            if (arith.isLeq(t)) {
                require(t, zusmt::dl_inequality(arith, t, true));
            } else if (arith.isNumEq(t)) {
                require(t, zusmt::dl_equality(arith, t, true));
            } else if (arith.isDisequality(t) && !args.empty() &&
                       arith.isSortNum(arith.getSortRef(args[0]))) {
                for (std::size_t i = 0; i < args.size(); ++i) {
                    for (std::size_t j = i + 1; j < args.size(); ++j) {
                        require_equality(args[i], args[j], t);
                    }
                }
            } else if (arith.isIte(t) && arith.isSortNum(arith.getSortRef(t))) {
                require_equality(t, args[1], t);
                require_equality(t, args[2], t);
            }
            for (opensmt::PTRef const arg : args) pending.push_back(arg);
        }
    }
};

struct SolverHandle {
    // Declaration order is destruction order, reversed: the interpreter (and
    // the solver it owns) goes first, then the config it refers to. Do not
    // reorder.
    std::unique_ptr<opensmt::SMTConfig> config;
    std::unique_ptr<RInterpret> interp;
    std::string logic;

    // Whether the solver's status still describes the current assertions.
    // OpenSMT leaves the status of the last check in place when formulas are
    // added, so without this a model, core or interpolant from an earlier
    // check would be reported for a problem that has since changed.
    bool result_current = false;
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

// How deeply SMT-LIB input may nest. Upstream's parser is iterative (a bison
// stack of 1Mi entries), but the term builder and the syntax tree's
// destructor both recurse once per level, and 100,000 levels of (not ...)
// overflowed the C stack. R turns a guard-page hit into an error only on some
// platforms, and even there it longjmps out of the C++ frames, abandoning the
// interpreter's state. 50,000 levels were measured to work on an 8 MB stack;
// this leaves a wide margin below that for smaller stacks and other compilers.
constexpr int kMaxNesting = 10000;

// One pass over the text, counting parentheses outside string literals,
// quoted symbols and comments -- the places where a parenthesis is not
// structure. Done before parsing, so nothing has been built yet to unwind.
void check_nesting(std::string const & script) {
    int depth = 0;
    enum { code, string_literal, quoted_symbol, comment } state = code;
    for (char const c : script) {
        switch (state) {
        case string_literal: if (c == '"') state = code; break;  // "" re-enters at once
        case quoted_symbol: if (c == '|') state = code; break;
        case comment: if (c == '\n') state = code; break;
        case code:
            if (c == '"') state = string_literal;
            else if (c == '|') state = quoted_symbol;
            else if (c == ';') state = comment;
            else if (c == '(') {
                if (++depth > kMaxNesting) {
                    throw zusmt::condition_error(
                        "zusmt_input_too_deep",
                        "SMT-LIB input is nested more than " + std::to_string(kMaxNesting) +
                            " levels deep; nothing was run. Name shared subterms with "
                            "define-fun or let rather than repeating them inline.");
                }
            } else if (c == ')') --depth;
            break;
        }
    }
}

// Commands that leave the assertions -- and so the last check's result --
// as they were. Anything not listed, including a failed command, is treated
// as having changed them: an unnecessary "call smt_check() again" is cheap,
// a model for the wrong problem is not.
bool keeps_result(opensmt::tokens::token command) {
    using namespace opensmt::tokens;
    switch (command) {
    case t_declaresort: case t_definesort: case t_declarefun: case t_declareconst:
    case t_definefun: case t_getassertions: case t_getassignment: case t_getinfo:
    case t_setinfo: case t_getoption: case t_getproof: case t_getunsatcore:
    case t_getvalue: case t_getmodel: case t_getinterpolants: case t_echo: case t_exit:
        return true;
    default:
        return false;
    }
}

// A diagnostic as one line per message: upstream ends each (error ...) with
// a blank line, which reads as a gap inside an R error message.
std::string tidy_diagnostic(std::string const & text) {
    std::string out;
    for (char const c : text) {
        if (c == '\n' && (out.empty() || out.back() == '\n')) continue;
        out += c;
    }
    while (!out.empty() && (out.back() == '\n' || out.back() == ' ')) out.pop_back();
    return out;
}

// Runs a script one command at a time and stops at the first that fails.
// Upstream's Interpret::execute() carries on past an error, so a caller who
// caught the error was left with a solver into which the *later* commands had
// already been asserted. Here the commands before the failing one have taken
// effect and the ones after it have not, and the error says which is which.
//
// Returns what the commands printed -- the output of (get-model), (echo ...)
// and so on -- for the R side to write. Printing it from here would call into
// R with C++ objects alive.
std::string run_script(SolverHandle & handle, std::string const & script) {
    check_nesting(script);

    // Smt2newContext takes a mutable char* because flex scans the buffer in place.
    std::vector<char> buffer(script.begin(), script.end());
    buffer.push_back('\0');

    zusmt::clear_error();
    opensmt::Smt2newContext context(buffer.data());
    {
        // Parse errors arrive as a non-zero status, the lexer's too (patch
        // rule 14 returns YYerror, so the parser frees its stack). Only a
        // flex internal error still throws (patch rule 4). Either way the
        // diagnostic is in the captured output, and nothing has run.
        CaptureScope capture;
        int parse_status = 0;
        std::string reason = "could not parse SMT-LIB input";
        try {
            parse_status = ::osmt_yyparse(&context);
        } catch (std::runtime_error const & e) {
            parse_status = -1;
            reason = e.what();
        }
        std::string const output = tidy_diagnostic(capture.take());
        if (parse_status != 0) {
            throw zusmt::condition_error("zusmt_syntax_error", output.empty() ? reason : output);
        }
    }

    opensmt::ASTNode const * root = context.getRoot();
    if (root == nullptr || root->children == nullptr) return "";
    std::vector<opensmt::ASTNode *> & commands = *root->children;

    std::string printed;
    std::size_t const total = commands.size();

    // Where a failure happened, and what that means for the rest.
    auto where = [&](std::size_t i, opensmt::tokens::token kind) {
        std::size_t const skipped = total - i - 1;
        std::string text = "in command " + std::to_string(i + 1) + " of " +
                           std::to_string(total) + " (" +
                           opensmt::tokens::tokenToName.at(kind) + ")";
        if (i > 0) text += "; the commands before it took effect";
        if (skipped > 0) {
            text += std::string(i > 0 ? " and " : "; ") + "the " +
                    (skipped == 1 ? std::string("command after it was")
                                  : std::to_string(skipped) + " commands after it were") +
                    " not run";
        }
        return text;
    };

    for (std::size_t i = 0; i < total && !handle.interp->gotExit(); ++i) {
        opensmt::tokens::token const kind = commands[i]->getToken().x;
        if (!keeps_result(kind)) handle.result_current = false;

        CaptureScope capture;
        zusmt::clear_error();
        try {
            handle.interp->command(*commands[i]);
        } catch (zusmt::condition_error const & e) {
            throw zusmt::condition_error(e.cls(), std::string(e.what()) + "\n" + where(i, kind),
                                         printed);
        }
        // Freed as upstream's execute() does; the context frees the rest.
        delete commands[i];
        commands[i] = nullptr;
        std::string const output = capture.take();

        // A semantic complaint arrives through notify_formatted(error = true),
        // which a patch rule has record itself. Asking the solver rather than
        // searching its output for "(error" matters in both directions:
        // (echo "(error ...)") succeeds and would otherwise be rejected, and a
        // change to upstream's error format would otherwise pass errors
        // through as success.
        if (zusmt::error_was_reported()) {
            std::string const diagnostic = tidy_diagnostic(output);
            throw zusmt::condition_error(
                "zusmt_smtlib_error",
                (diagnostic.empty() ? std::string("the solver reported an error") : diagnostic) +
                    "\n" + where(i, kind),
                printed);
        }
        printed += output;
        if (kind == opensmt::tokens::t_checksat) handle.result_current = true;
    }
    return printed;
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

// Strings crossing into the solver, and back.
//
// SMT-LIB symbols are not ASCII-only: |naïve| is a legal quoted symbol, and it
// reaches the solver, the model, the unsat core and the interpolant unchanged.
// So both directions need an encoding that is declared rather than inherited
// from whatever locale the session happens to run in.
//
// In:  Rf_translateCharUTF8() rather than CHAR(), so the solver is always
//      handed UTF-8 regardless of how R marked the string.
// Out: Rf_mkCharCE(..., CE_UTF8) rather than Rf_mkChar(), so R is told what it
//      is being given instead of assuming native. (R stores pure ASCII
//      unmarked either way, so this costs nothing in the common case.)
//
// On a UTF-8 session the old code round-tripped correctly by coincidence --
// the bytes passed through and the locale agreed. That is the part worth
// fixing: it was right by accident, not by construction.
//
// Both can longjmp -- an invalid string, or allocation failing -- and both
// are called with C++ objects alive, so both go through unwind_protect().
char const * utf8_arg(SEXP x, R_xlen_t i = 0) {
    char const * out = nullptr;
    zusmt::unwind_protect([&]() -> SEXP {
        out = Rf_translateCharUTF8(STRING_ELT(x, i));
        return R_NilValue;
    });
    return out;
}

SEXP utf8_string(std::string const & text) {
    char const * const chars = text.c_str();
    return zusmt::unwind_protect([&]() -> SEXP { return Rf_mkCharCE(chars, CE_UTF8); });
}

// The remaining allocations made while C++ objects are alive.
SEXP alloc_vector(SEXPTYPE type, R_xlen_t n) {
    return zusmt::unwind_protect([&]() -> SEXP { return Rf_allocVector(type, n); });
}

SEXP scalar_string(std::string const & text) {
    char const * const chars = text.c_str();
    return zusmt::unwind_protect(
        [&]() -> SEXP { return Rf_ScalarString(Rf_mkCharCE(chars, CE_UTF8)); });
}

// The interpolant and core entry points refuse a result whose assertions have
// changed since the check that produced it. See SolverHandle::result_current.
void require_current(SolverHandle const & handle, char const * what) {
    if (!handle.result_current) {
        throw zusmt::condition_error(
            "zusmt_stale_result",
            std::string("the assertions have changed since the last check, so its ") + what +
                " no longer describes them; call smt_check() again first");
    }
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

        // The R object first, while nothing on the C++ side exists: an
        // allocation that longjmps here leaks nothing, and once the handle is
        // built it only has to be stored, which cannot fail.
        SEXP xp = PROTECT(R_MakeExternalPtr(nullptr, solver_tag(), R_NilValue));
        R_RegisterCFinalizerEx(xp, finalize_solver, TRUE);

        std::string const name(CHAR(STRING_ELT(logic_name, 0)));
        check_logic_supported(name);

        auto handle = std::make_unique<SolverHandle>();
        handle->logic = name;
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
        handle->interp->difference_logic = name == "QF_IDL" || name == "QF_RDL";

        // The interpreter builds its logic and solver when it sees set-logic,
        // so the handle is not usable until this runs. Doing it here means a
        // bad logic name fails at solver_new() rather than at the first
        // assert.
        (void) run_script(*handle, "(set-logic " + name + ")");

        R_SetExternalPtrAddr(xp, handle.release());  // ownership now belongs to the finalizer
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
        opensmt::PTRef var = handle->interp->theLogic().mkBoolVar(utf8_arg(name));
        opensmt::Logic & logic = handle->interp->theLogic();
        opensmt::PTRef term = (Rf_asLogical(negated) == TRUE) ? logic.mkNot(var) : var;
        handle->result_current = false;
        handle->interp->getMainSolver().insertFormula(term);
        return R_NilValue;
    });
}

extern "C" SEXP C_solver_check(SEXP xp, SEXP timeout) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);

        if (TYPEOF(timeout) != REALSXP || Rf_length(timeout) != 1) {
            throw std::invalid_argument("`timeout` must be a single number");
        }
        // ISNAN, not ISNA: NA_real_ and NaN are both unusable here, and
        // ISNA() is true only of the first. `seconds <= 0` is false for NaN,
        // so an unchecked NaN would arm a deadline that never expires.
        double const seconds = REAL(timeout)[0];
        if (ISNAN(seconds) || seconds <= 0) {
            throw std::invalid_argument(
                "`timeout` must be a positive number of seconds, or Inf for no limit");
        }

        // Cleared on the way out whatever happens: a deadline left armed
        // would silently bound the *next* solve on this handle, and an
        // exception out of check() must not leave that behind.
        struct DeadlineScope {
            explicit DeadlineScope(double seconds) {
                if (R_FINITE(seconds)) zusmt::set_deadline(seconds);
            }
            ~DeadlineScope() { zusmt::clear_deadline(); }
        } const deadline{seconds};

        zusmt::clear_interrupt_request();
        opensmt::sstat const status = handle->interp->getMainSolver().check();
        handle->result_current = true;

        char const * result = "error";
        if (status == opensmt::s_Undef && zusmt::deadline_reached()) {
            // Both halves matter. The clock alone is not enough: the deadline
            // can expire between the last okContinue() and the solver
            // finishing, and a decided answer is valid however late it is --
            // reporting "unknown" there would throw away a correct sat or
            // unsat. An undecided status alone is not enough either, because
            // a solver may legitimately return s_Undef without any deadline.
            //
            // Before the interrupt branch: should_stop() checks the deadline
            // first, so when both fired the deadline is what ended the
            // search, and reporting the interrupt would name the wrong cause.
            result = "timeout";
        } else if (zusmt::interrupt_was_requested()) {
            // Unlike the deadline, this is not guarded on s_Undef, and the
            // asymmetry is deliberate. A late deadline is the solver's own
            // bound and a decided answer beats it; an interrupt is the user
            // asking for control back, and R's contract for Ctrl-C is that the
            // call is abandoned rather than returning a value -- even if the
            // search happened to finish in the same instant.
            //
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
// Returns what the script printed, which smt_assert() writes to the console.
extern "C" SEXP C_solver_run(SEXP xp, SEXP text) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);
        if (TYPEOF(text) != STRSXP || Rf_length(text) != 1) {
            throw std::invalid_argument("SMT-LIB input must be a single string");
        }
        std::string const printed = run_script(*handle, utf8_arg(text));
        return scalar_string(printed);
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
        require_current(*handle, "model");

        opensmt::Logic & logic = handle->interp->theLogic();
        auto * arith = dynamic_cast<opensmt::ArithLogic *>(&logic);
        std::unique_ptr<opensmt::Model> model = solver.getModel();

        // Each symbol once. Upstream records a declaration every time it
        // sees one, and re-declaring a constant with the same sort is
        // accepted and yields the same symbol -- which reported it twice, a
        // list with duplicate names on which m$x silently picks the first.
        std::vector<opensmt::SymRef> reported;
        {
            std::unordered_set<uint32_t> seen;
            for (opensmt::SymRef sym : handle->interp->declarations()) {
                if (logic.getSym(sym).nargs() == 0 && seen.insert(sym.x).second) {
                    reported.push_back(sym);
                }
            }
        }

        // Allocated at its final size so every value is stored straight into
        // a protected list. Collecting SEXPs in a std::vector on the way
        // would leave them unprotected: R's collector cannot see C++
        // containers, and this loop allocates repeatedly.
        R_xlen_t const n = static_cast<R_xlen_t>(reported.size());
        SEXP out = PROTECT(alloc_vector(VECSXP, n));
        SEXP names = PROTECT(alloc_vector(STRSXP, n));

        for (R_xlen_t at = 0; at < n; ++at) {
            opensmt::SymRef const sym = reported[static_cast<std::size_t>(at)];
            opensmt::PTRef const term = logic.mkUninterpFun(sym, {});
            opensmt::PTRef const value = model->evaluate(term);
            SET_STRING_ELT(names, at, utf8_string(logic.getSymName(sym)));

            if (value == logic.getTerm_true()) {
                SET_VECTOR_ELT(out, at, Rf_ScalarLogical(TRUE));
            } else if (value == logic.getTerm_false()) {
                SET_VECTOR_ELT(out, at, Rf_ScalarLogical(FALSE));
            } else if (arith != nullptr && arith->isNumConst(value)) {
                // A double loses exactness, and SMT rationals routinely are
                // not representable in one. Report the double for arithmetic,
                // and the exact value as an attribute for anyone who needs it.
                opensmt::Number const & number = arith->getNumConst(value);
                double const approx = number.get_d();
                std::string const exact = number.get_str();
                char const * const exact_chars = exact.c_str();
                SET_VECTOR_ELT(out, at, zusmt::unwind_protect([&]() -> SEXP {
                    SEXP num = PROTECT(Rf_ScalarReal(approx));
                    // Protected before the call: exact_tag() installs its
                    // symbol on first use, which can allocate, and argument
                    // evaluation order is unspecified (rchk flags it).
                    SEXP exact_value = PROTECT(Rf_ScalarString(Rf_mkCharCE(exact_chars, CE_UTF8)));
                    Rf_setAttrib(num, exact_tag(), exact_value);
                    UNPROTECT(2);
                    return num;
                }));
            } else {
                // Anything else -- an uninterpreted sort's value, say --
                // comes back as the solver's own printed form rather than
                // being coerced into an R type it does not fit.
                SET_VECTOR_ELT(out, at, scalar_string(logic.printTerm(value)));
            }
        }

        zusmt::unwind_protect([&]() -> SEXP {
            Rf_setAttrib(out, R_NamesSymbol, names);
            return R_NilValue;
        });
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
        require_current(*handle, "unsat core");

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

        SEXP out = PROTECT(alloc_vector(STRSXP, terms.size()));
        SEXP names = R_NilValue;
        int named = 0;

        for (int i = 0; i < terms.size(); ++i) {
            std::string const * const name = term_names.tryGetNameForTerm(terms[i]);
            if (want_full) {
                SET_STRING_ELT(out, i, utf8_string(logic.printTerm(terms[i])));
            } else {
                // Without :print-cores-full every term here is a named one,
                // so this lookup cannot fail -- but a null would be a silent
                // empty string, so say so instead.
                if (name == nullptr) {
                    UNPROTECT(1);
                    throw std::runtime_error("the solver reported an unnamed term in a named core");
                }
                SET_STRING_ELT(out, i, utf8_string(*name));
            }
            if (name != nullptr) ++named;
        }

        // Names as an R attribute, so a caller who used (! ... :named n) gets
        // them back without giving up the terms. Omitted entirely when
        // nothing was named, rather than filling a vector with "".
        if (want_full && named > 0) {
            names = PROTECT(alloc_vector(STRSXP, terms.size()));
            for (int i = 0; i < terms.size(); ++i) {
                std::string const * const name = term_names.tryGetNameForTerm(terms[i]);
                SET_STRING_ELT(names, i, utf8_string(name != nullptr ? *name : std::string()));
            }
            zusmt::unwind_protect([&]() -> SEXP {
                Rf_setAttrib(out, R_NamesSymbol, names);
                return R_NilValue;
            });
            UNPROTECT(1);
        }

        UNPROTECT(1);
        return out;
    });
}

// A Craig interpolant for a partition of the assertions.
//
// Given A and B whose conjunction is unsatisfiable, an interpolant I follows
// from A, is inconsistent with B, and mentions only symbols the two share.
// `names` picks which named assertions form A; everything else asserted is B.
//
// Named assertions are the interface because that is how the solver tracks
// them: each top-level assertion has an index, and a partition is a bitmask
// over those indices. Reaching them by name through TermNames is what the
// (get-interpolants) command does too -- but that one prints its result,
// which is why this exists.
extern "C" SEXP C_solver_interpolant(SEXP xp, SEXP names) {
    return zusmt::with_firewall([&]() -> SEXP {
        SolverHandle * handle = handle_from(xp);

        if (TYPEOF(names) != STRSXP || Rf_length(names) == 0) {
            throw std::invalid_argument("`a` must be a character vector of assertion names");
        }
        if (!handle->config->produce_inter()) {
            throw std::runtime_error(
                "interpolation was not enabled; create the solver with "
                "smt_solver(interpolants = TRUE)");
        }

        opensmt::MainSolver & solver = handle->interp->getMainSolver();
        if (solver.getStatus() != opensmt::s_False) {
            throw std::runtime_error(
                "an interpolant is only available after an unsatisfiable check");
        }
        require_current(*handle, "proof");

        opensmt::TermNames const & term_names = std::as_const(solver).getTermNames();
        opensmt::ipartitions_t mask = 0;

        for (R_xlen_t i = 0; i < Rf_length(names); ++i) {
            std::string const name(utf8_arg(names, i));

            std::optional<opensmt::PTRef> const term = term_names.tryGetTermByName(name);
            if (!term.has_value()) {
                throw std::invalid_argument(
                    "no assertion is named '" + name +
                    "'; name them with (! ... :named " + name + ")");
            }
            // A named term that is not itself a top-level assertion has no
            // index, so there is no partition bit to set for it. Saying so
            // beats computing an interpolant against a silently smaller A.
            if (!handle->interp->is_top_level_assertion(*term)) {
                throw std::invalid_argument(
                    "'" + name + "' names a term that is not a top-level assertion");
            }
            int const index = handle->interp->get_assertion_index(*term);
            if (index < 0) {
                throw std::runtime_error("'" + name + "' has no assertion index");
            }
            opensmt::setbit(mask, static_cast<unsigned>(index));
        }

        // OpenSMT interpolates QF_UF, QF_LIA and QF_LRA. For the other logics
        // it fails in three different ways -- "Not implemented yet" on about
        // half of QF_IDL/QF_RDL problems, "Interpolation not supported yet" on
        // QF_AX, and an internal-sounding theory-solver message on
        // QF_UFLIA/QF_UFLRA -- none of which says what the caller needs to
        // know. One classed error does, with upstream's text kept for detail.
        opensmt::vec<opensmt::PTRef> interpolants;
        try {
            std::unique_ptr<opensmt::InterpolationContext> context = solver.getInterpolationContext();
            context->getSingleInterpolant(interpolants, mask);
        } catch (std::exception const & e) {
            throw zusmt::condition_error(
                "zusmt_unsupported_input",
                "interpolation is not supported for this " + handle->logic +
                    " problem (OpenSMT: " + tidy_diagnostic(e.what()) +
                    "). Interpolants are available for QF_UF, QF_LIA and QF_LRA, and only "
                    "for some QF_IDL and QF_RDL problems.");
        }

        opensmt::Logic & logic = handle->interp->theLogic();
        SEXP out = PROTECT(alloc_vector(STRSXP, interpolants.size()));
        for (int i = 0; i < interpolants.size(); ++i) {
            // printTerm(), as the unsat core uses, not pp(): pp() writes
            // numerals as -1 and 16/5, which no SMT-LIB parser but OpenSMT's
            // accepts. printTerm() writes (- 1) and (/ 16 5), so an interpolant
            // can be handed to another solver as it stands.
            SET_STRING_ELT(out, i, utf8_string(logic.printTerm(interpolants[i])));
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
        handle->result_current = false;

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
