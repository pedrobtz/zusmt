// Stage 1 of roadmap.md: a probe, not a feature.
//
// OpenSMT needs three things from the build: C++20, GMP with its C++
// bindings, and threads. This file exercises all three through R's own
// toolchain so that the CI matrix answers "can this package be built where
// it has to be built?" before ~90 vendored translation units depend on the
// answer. It is expected to be deleted once the vendored solver replaces it.

#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

#include <gmpxx.h>

#include <cstdio>
#include <string>
#include <thread>

static_assert(__cplusplus >= 202002L, "zusmt requires C++20");

namespace {

// A concept, so that the probe fails on a compiler merely *claiming* C++20.
template <typename T>
concept Addable = requires(T a, T b) { a + b; };

template <Addable T>
T add(T a, T b) {
    return a + b;
}

std::string compiler_id() {
#if defined(__clang__)
    return std::string("clang ") + __clang_version__;
#elif defined(__GNUC__)
    return std::string("gcc ") + __VERSION__;
#else
    return std::string("unknown");
#endif
}

}  // namespace

// The exception firewall this package will use everywhere (roadmap Stage 5):
// nothing with a destructor may be alive when Rf_error() longjmps past it, so
// the message is copied into a plain buffer and every C++ object is destroyed
// before the error is raised.
extern "C" SEXP C_toolchain_report(void) {
    char errbuf[256] = "";
    char rational[64] = "";
    char gmpver[64] = "";
    char compiler[192] = "";
    int threads_ok = 0;

    {
        try {
            mpq_class a(1, 3), b(1, 6), c;
            std::string s;
            std::thread t([&] {
                c = add(a, b);
                c.canonicalize();
                s = c.get_str();
            });
            t.join();
            threads_ok = 1;
            std::snprintf(rational, sizeof rational, "%s", s.c_str());
            std::snprintf(gmpver, sizeof gmpver, "%s", gmp_version);
            std::snprintf(compiler, sizeof compiler, "%s", compiler_id().c_str());
        } catch (std::exception const & e) {
            std::snprintf(errbuf, sizeof errbuf, "%s", e.what());
        } catch (...) {
            std::snprintf(errbuf, sizeof errbuf, "unknown C++ exception");
        }
    }

    if (errbuf[0] != '\0') {
        Rf_error("zusmt toolchain probe failed: %s", errbuf);
    }

    char const * names[] = {"cplusplus", "compiler", "gmp_version",
                            "rational", "threads_ok", ""};
    SEXP out = PROTECT(Rf_mkNamed(VECSXP, names));
    SET_VECTOR_ELT(out, 0, Rf_ScalarReal(static_cast<double>(__cplusplus)));
    SET_VECTOR_ELT(out, 1, Rf_mkString(compiler));
    SET_VECTOR_ELT(out, 2, Rf_mkString(gmpver));
    SET_VECTOR_ELT(out, 3, Rf_mkString(rational));
    SET_VECTOR_ELT(out, 4, Rf_ScalarLogical(threads_ok));
    UNPROTECT(1);
    return out;
}
