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

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <cstdio>
#include <exception>
#include <string>
#include <utility>

namespace zusmt {

// Wraps the body of a .Call entry point. The body returns a SEXP and may
// throw; anything it throws becomes an R error raised after the body's frame
// -- and everything in it -- has been destroyed.
template <typename Body>
SEXP with_firewall(Body && body) {
    char message[512] = "";
    SEXP result = R_NilValue;

    {
        try {
            result = std::forward<Body>(body)();
        } catch (std::exception const & e) {
            std::snprintf(message, sizeof message, "%s", e.what());
        } catch (...) {
            std::snprintf(message, sizeof message, "unidentified C++ exception");
        }
    }

    // Nothing with a destructor is alive here, so the longjmp is safe.
    if (message[0] != '\0') {
        Rf_error("%s", message);
    }
    return result;
}

}  // namespace zusmt

#endif  // ZUSMT_BOUNDARY_H
