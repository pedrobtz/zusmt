#ifndef ZUSMT_BOUNDARY_H
#define ZUSMT_BOUNDARY_H

// The one place where C++ and R meet.
//
// Two rules, and every entry point in this package obeys both:
//
//   1. No C++ exception may reach R. R's error mechanism is longjmp and knows
//      nothing about unwinding; an exception crossing into R's C code is
//      undefined behaviour. So every entry point runs its body inside
//      with_firewall(), which converts an exception into an ordinary R
//      condition.
//
//   2. No longjmp may cross a live C++ frame. Rf_error() and
//      R_CheckUserInterrupt() both longjmp, skipping destructors on the way,
//      which would leak the solver's heap. So the firewall copies the message
//      into a plain buffer and lets every C++ object go out of scope *before*
//      raising, and interrupts are polled rather than delivered (see
//      zusmt::interrupt_requested in r_compat.h).
//
//      Rule 2 also covers R API calls that *can* longjmp on their own --
//      allocation on memory exhaustion, Rf_translateCharUTF8() on an invalid
//      string. Where one is made while a C++ object is alive, it goes through
//      unwind_protect(), which turns R's longjmp into a C++ exception, lets
//      the C++ frames unwind, and resumes R's unwind from the firewall.

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <csetjmp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace zusmt {

// An error that R should see with a class of its own, so callers can tell
// "your input is outside what this logic supports" from "the solver failed"
// with tryCatch() rather than by matching message text. `output` is anything
// the solver printed before the failure -- the (echo ...) of an earlier
// command, say -- which is written to the console before the error is raised.
class condition_error : public std::runtime_error {
public:
    condition_error(std::string cls, std::string const & message, std::string output = "")
        : std::runtime_error(message), cls_(std::move(cls)), output_(std::move(output)) {}
    char const * cls() const noexcept { return cls_.c_str(); }
    std::string const & output() const noexcept { return output_; }

private:
    std::string cls_;
    std::string output_;
};

// Thrown by unwind_protect() when R longjmp'd inside the protected call; the
// firewall resumes R's unwind with the token once the C++ frames are gone.
struct unwind_exception {
    SEXP token;
};

inline SEXP unwind_token() {
    static SEXP token = [] {
        SEXP t = R_MakeUnwindCont();
        R_PreserveObject(t);
        return t;
    }();
    return token;
}

// Runs an R API call that may longjmp. The callable's own frame must hold
// nothing with a destructor -- it is the one frame the longjmp still crosses.
// Same design as cpp11::unwind_protect().
template <typename Fun>
SEXP unwind_protect(Fun && fun) {
    SEXP const token = unwind_token();
    std::jmp_buf jmpbuf;
    if (setjmp(jmpbuf)) {
        throw unwind_exception{token};
    }
    using F = std::remove_reference_t<Fun>;
    SEXP result = R_UnwindProtect(
        [](void * data) -> SEXP { return (*static_cast<F *>(data))(); },
        static_cast<void *>(&fun),
        [](void * buf, Rboolean jump) {
            if (jump == TRUE) std::longjmp(*static_cast<std::jmp_buf *>(buf), 1);
        },
        static_cast<void *>(&jmpbuf), token);
    // Release the value R_UnwindProtect stored in the token, as cpp11 does.
    SETCAR(token, R_NilValue);
    return result;
}

// Raises an R condition of class c(cls, "zusmt_error", "error", "condition"),
// through an R helper so the condition's call is the exported function the
// user called rather than .Call(). Longjmps; call it with no C++ object alive.
[[noreturn]] inline void raise_condition(char const * cls, char const * message,
                                         char const * output) {
    SEXP ns = PROTECT(R_FindNamespace(Rf_mkString("zusmt")));
    SEXP fun = PROTECT(Rf_findFun(Rf_install("raise_condition"), ns));
    SEXP cls_s = PROTECT(Rf_ScalarString(Rf_mkCharCE(cls, CE_UTF8)));
    SEXP msg_s = PROTECT(Rf_ScalarString(Rf_mkCharCE(message, CE_UTF8)));
    SEXP out_s = PROTECT(Rf_ScalarString(Rf_mkCharCE(output, CE_UTF8)));
    SEXP call = PROTECT(Rf_lang4(fun, cls_s, msg_s, out_s));
    Rf_eval(call, ns);
    UNPROTECT(6);
    Rf_error("%s", message);  // unreachable: raise_condition() always signals
}

// Wraps the body of a .Call entry point. The body returns a SEXP and may
// throw; anything it throws becomes an R error raised after the body's frame
// -- and everything in it -- has been destroyed.
template <typename Body>
SEXP with_firewall(Body && body) {
    char message[4096] = "";
    char cls[64] = "";
    // malloc rather than std::string: it must outlive the try block, and
    // nothing with a destructor may be alive when the condition is raised.
    char * output = nullptr;
    SEXP unwind = nullptr;
    SEXP result = R_NilValue;

    {
        try {
            result = std::forward<Body>(body)();
        } catch (unwind_exception const & e) {
            unwind = e.token;
        } catch (condition_error const & e) {
            std::snprintf(message, sizeof message, "%s", e.what());
            std::snprintf(cls, sizeof cls, "%s", e.cls());
            output = static_cast<char *>(std::malloc(e.output().size() + 1));
            if (output != nullptr) std::memcpy(output, e.output().c_str(), e.output().size() + 1);
        } catch (std::exception const & e) {
            std::snprintf(message, sizeof message, "%s", e.what());
        } catch (...) {
            std::snprintf(message, sizeof message, "unidentified C++ exception");
        }
    }

    // Nothing with a destructor is alive here, so the longjmp is safe.
    if (unwind != nullptr) {
        R_ContinueUnwind(unwind);
    }
    if (cls[0] != '\0') {
        // Copied onto R's transient heap so the malloc'd block can be freed
        // before anything that longjmps; R_alloc'd memory is reclaimed by R
        // however the call ends.
        char const * out = "";
        if (output != nullptr) {
            std::size_t const n = std::strlen(output) + 1;
            char * copy = R_alloc(n, 1);
            std::memcpy(copy, output, n);
            std::free(output);
            out = copy;
        }
        raise_condition(cls, message, out);
    }
    std::free(output);
    if (message[0] != '\0') {
        Rf_error("%s", message);
    }
    return result;
}

}  // namespace zusmt

#endif  // ZUSMT_BOUNDARY_H
