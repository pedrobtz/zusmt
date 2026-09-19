#include "r_compat.h"

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <chrono>
#include <streambuf>
#include <string>

namespace {

// Line-buffered: Rprintf on every character would be slow and would interleave
// badly with R's own output. Buffered text is flushed on newline and on sync.
bool & error_flag() {
    static bool reported = false;
    return reported;
}

bool & capturing() {
    static bool on = false;
    return on;
}

std::string & capture_buffer() {
    static std::string buffer;
    return buffer;
}

class RStreamBuf : public std::streambuf {
public:
    explicit RStreamBuf(bool to_stderr) : to_stderr_(to_stderr) {}

protected:
    int overflow(int c) override {
        if (c == EOF) {
            emit();
            return 0;
        }
        pending_.push_back(static_cast<char>(c));
        if (c == '\n') emit();
        return c;
    }

    std::streamsize xsputn(char const * s, std::streamsize n) override {
        pending_.append(s, static_cast<std::string::size_type>(n));
        if (pending_.find('\n') != std::string::npos) emit();
        return n;
    }

    int sync() override {
        emit();
        return 0;
    }

private:
    void emit() {
        if (pending_.empty()) return;
        if (capturing()) {
            capture_buffer() += pending_;
            pending_.clear();
            return;
        }
        // "%s" rather than passing the text as the format: solver output
        // contains user-supplied symbol names, which may contain '%'.
        if (to_stderr_) {
            REprintf("%s", pending_.c_str());
        } else {
            Rprintf("%s", pending_.c_str());
        }
        pending_.clear();
    }

    bool to_stderr_;
    std::string pending_;
};

std::ostream & stream_for(bool to_stderr) {
    if (to_stderr) {
        static RStreamBuf buf(true);
        static std::ostream stream(&buf);
        return stream;
    }
    static RStreamBuf buf(false);
    static std::ostream stream(&buf);
    return stream;
}

// xorshift32. Seeded to a fixed non-zero constant so that a solve is
// reproducible across sessions unless upstream reseeds it.
unsigned int & rng_state() {
    static unsigned int state = 2463534242u;
    return state;
}

}  // namespace

std::ostream & zusmt::rout() {
    return stream_for(false);
}

std::ostream & zusmt::rerr() {
    return stream_for(true);
}

void zusmt::fatal(char const * what) {
    throw std::runtime_error(what != nullptr ? what : "zusmt: fatal error in the bundled solver");
}

int zusmt::pseudo_rand() {
    unsigned int & x = rng_state();
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return static_cast<int>(x & 0x7fffffffu);
}

void zusmt::pseudo_srand(unsigned int seed) {
    rng_state() = (seed == 0u) ? 2463534242u : seed;
}

int zusmt::alloc_printf(char ** out, char const * fmt, ...) {
    if (out == nullptr) return -1;
    *out = nullptr;

    va_list args;
    va_start(args, fmt);
    va_list measure;
    va_copy(measure, args);
    int const needed = std::vsnprintf(nullptr, 0, fmt, measure);
    va_end(measure);

    if (needed < 0) {
        va_end(args);
        return -1;
    }

    // malloc, not new: the vendored call sites free() what they get back.
    char * buffer = static_cast<char *>(std::malloc(static_cast<std::size_t>(needed) + 1));
    if (buffer == nullptr) {
        va_end(args);
        return -1;
    }

    int const written = std::vsnprintf(buffer, static_cast<std::size_t>(needed) + 1, fmt, args);
    va_end(args);

    if (written < 0) {
        std::free(buffer);
        return -1;
    }
    *out = buffer;
    return written;
}

namespace {

// R_ToplevelExec runs the function with a fresh top-level context, so a
// longjmp out of R_CheckUserInterrupt unwinds to that context rather than
// past our C++ frames, and is reported as FALSE instead.
void check_interrupt_inner(void *) {
    R_CheckUserInterrupt();
}

bool & interrupt_seen() {
    static bool seen = false;
    return seen;
}

// -1 means unarmed. Counts polls down to zero, then reports pending once.
int & test_interrupt_countdown() {
    static int countdown = -1;
    return countdown;
}

}  // namespace

bool zusmt::interrupt_requested() {
    // Throttled: R_ToplevelExec sets up and tears down a context, and
    // okContinue() is called in the solver's innermost loop. 50ms is far below
    // what a person notices and far above what this costs.
    using clock = std::chrono::steady_clock;
    static clock::time_point last;
    static bool primed = false;

    if (interrupt_seen()) return true;

    int & countdown = test_interrupt_countdown();
    if (countdown >= 0) {
        if (countdown == 0) {
            countdown = -1;
            interrupt_seen() = true;
            return true;
        }
        --countdown;
        return false;
    }

    clock::time_point const now = clock::now();
    if (primed && now - last < std::chrono::milliseconds(50)) return false;
    primed = true;
    last = now;

    if (R_ToplevelExec(check_interrupt_inner, nullptr) == FALSE) {
        interrupt_seen() = true;
        return true;
    }
    return false;
}

bool zusmt::interrupt_was_requested() {
    return interrupt_seen();
}

void zusmt::clear_interrupt_request() {
    interrupt_seen() = false;
}

void zusmt::arm_test_interrupt(int polls) {
    test_interrupt_countdown() = polls;
    interrupt_seen() = false;
}

void zusmt::begin_capture() {
    capture_buffer().clear();
    capturing() = true;
}

std::string zusmt::end_capture() {
    // Flush whatever is buffered but unterminated: the solver's last line may
    // have no trailing newline, and it is usually the interesting one.
    zusmt::rout().flush();
    zusmt::rerr().flush();
    capturing() = false;
    std::string out = capture_buffer();
    capture_buffer().clear();
    return out;
}

void zusmt::note_error() {
    error_flag() = true;
}

void zusmt::clear_error() {
    error_flag() = false;
}

bool zusmt::error_was_reported() {
    return error_flag();
}
