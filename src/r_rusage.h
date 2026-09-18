#ifndef ZUSMT_R_RUSAGE_H
#define ZUSMT_R_RUSAGE_H

// getrusage() for Windows.
//
// The vendored solver times itself with getrusage(RUSAGE_SELF) from
// <sys/resource.h>, which mingw does not have -- the header does not exist,
// so the Windows build fails at the first file that includes Timer.h.
// tools/patches.sh redirects those includes here.

#if defined(_WIN32)

#include <ctime>
#include <sys/time.h>  // mingw declares struct timeval here

#ifndef RUSAGE_SELF
#define RUSAGE_SELF 0
#endif

// mingw declares struct timeval (with a long tv_usec) but not the POSIX
// suseconds_t typedef that upstream's BTime uses for its microsecond field.
// Repeating an identical typedef is legal in C++, so this stays safe if a
// future mingw does declare it as long.
typedef long suseconds_t;

struct rusage {
    struct timeval ru_utime;
    struct timeval ru_stime;
};

// clock() rather than GetProcessTimes(): <windows.h> would reach 87
// translation units of upstream C++ and bring its min/max and ERROR macros
// with it. The cost is that Windows CRT clock() measures elapsed time since
// process start rather than CPU time, and that system time is reported as
// zero. These values feed the solver's statistics and verbosity output, never
// a result, so an approximation is the right trade here.
inline int getrusage(int /* who */, struct rusage * usage) {
    if (usage == nullptr) return -1;
    double const seconds = static_cast<double>(std::clock()) / static_cast<double>(CLOCKS_PER_SEC);
    long const whole = static_cast<long>(seconds);
    usage->ru_utime.tv_sec = whole;
    usage->ru_utime.tv_usec = static_cast<long>((seconds - static_cast<double>(whole)) * 1000000.0);
    usage->ru_stime.tv_sec = 0;
    usage->ru_stime.tv_usec = 0;
    return 0;
}

#else

#include <sys/resource.h>
#include <sys/time.h>

#endif  // _WIN32

#endif  // ZUSMT_R_RUSAGE_H
