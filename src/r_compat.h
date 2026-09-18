#ifndef ZUSMT_R_COMPAT_H
#define ZUSMT_R_COMPAT_H

// Shim for the vendored OpenSMT sources.
//
// A library embedded in R may not write to stdout/stderr directly, call the
// system RNG, or terminate the process -- `R CMD check` checks the shared
// object for those symbols and CRAN rejects them. Upstream is a command-line
// solver and does all three. tools/patches.sh rewrites the vendored tree to
// use what is declared here, mechanically and reproducibly, so a version bump
// re-applies the same rules rather than re-deriving them from a check log.

#include <ostream>

// Format-checking for the printf-style shim below. The archetype has to be
// chosen per compiler, not per __GNUC__: clang defines __GNUC__ but does not
// implement the gnu_printf archetype, and warns -Wignored-attributes if given
// it. On mingw the distinction is the point -- plain `printf` there checks
// against MSVCRT's conversions, while the shim forwards to std::vsnprintf.
#if defined(__clang__)
#  define ZUSMT_PRINTF_FMT(fmt_idx, first_arg) \
     __attribute__((format(printf, fmt_idx, first_arg)))
#elif defined(__GNUC__)
#  define ZUSMT_PRINTF_FMT(fmt_idx, first_arg) \
     __attribute__((format(gnu_printf, fmt_idx, first_arg)))
#else
#  define ZUSMT_PRINTF_FMT(fmt_idx, first_arg)
#endif

// Declares Rprintf/REprintf, which rule 2 in tools/patches.sh rewrites calls
// to. R_ext/Print.h alone does not pull in the Rf_ remapping macros.
#include <R_ext/Print.h>

namespace zusmt {

// Replace std::cout / std::cerr. These write through Rprintf/REprintf, so
// output reaches the R console (and sinks, and knitr) rather than fd 1/2.
std::ostream & rout();
std::ostream & rerr();

// Replaces exit()/abort(). Throws, so C++ destructors run and the R boundary
// turns it into an ordinary R condition; the process survives, which is the
// whole point -- a solver bug must not take the user's session with it.
[[noreturn]] void fatal(char const * what);

// Replaces GNU asprintf(), which mingw does not have. Named alloc_printf
// rather than asprintf so the shim never shadows the libc name. Same contract: prints
// into a freshly malloc'd buffer that the caller free()s, returns the length
// or -1. Used on every platform rather than only Windows, so the vendored
// call sites have one behaviour everywhere.
int alloc_printf(char ** out, char const * fmt, ...) ZUSMT_PRINTF_FMT(2, 3);

// Interrupt polling for the solver's search loop.
//
// R delivers an interrupt by longjmp, which must never cross a C++ frame:
// destructors would be skipped and the solver's heap left behind. So the
// search loop asks this instead. interrupt_requested() checks for a *pending*
// interrupt without delivering it (R_ToplevelExec catches the longjmp), and
// tools/patches.sh wires it into CoreSMTSolver::okContinue(), so a request
// ends the search the same way upstream's own stop flag does -- by unwinding
// normally, through every destructor.
//
// The boundary then calls R_CheckUserInterrupt() once no C++ frame is left,
// which is what actually raises the condition in R.
bool interrupt_requested();

// Test hook. Makes the next solve behave as though the user pressed Ctrl-C
// after `polls` polls of the search loop, so the wiring -- okContinue()
// stopping the search, the search unwinding, the boundary reporting it -- can
// be tested deterministically and on every platform. It does not exercise
// R_ToplevelExec itself; nothing in a test can press Ctrl-C portably.
void arm_test_interrupt(int polls);

// True when a poll during the last solve saw a pending interrupt.
bool interrupt_was_requested();
void clear_interrupt_request();

// Replaces rand()/srand(). Self-contained xorshift rather than R's RNG: these
// call sites are heuristic tie-breaks inside the solver, and reaching into
// R's RNG stream from solver internals would both need GetRNGstate/PutRNGstate
// around every solve and perturb reproducibility of the user's own draws.
int pseudo_rand();
void pseudo_srand(unsigned int seed);

}  // namespace zusmt

#endif  // ZUSMT_R_COMPAT_H
