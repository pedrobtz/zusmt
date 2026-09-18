#include "r_compat.h"

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <streambuf>
#include <string>

namespace {

// Line-buffered: Rprintf on every character would be slow and would interleave
// badly with R's own output. Buffered text is flushed on newline and on sync.
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

int zusmt::asprintf(char ** out, char const * fmt, ...) {
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
