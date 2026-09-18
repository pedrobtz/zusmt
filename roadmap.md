# zusmt roadmap

Goal: vendor [OpenSMT](https://github.com/usi-verification-and-security/opensmt) into an R package
that installs from source on Linux, macOS and Windows with R's own toolchain, and expose an SMT
solving API to R.

## What we are vendoring

Facts established from upstream `master` (commit `15b42c6`, version 2.9.3, Dec 2025) — re-verify when
pinning a tag:

| Property | Value | Consequence for us |
| --- | --- | --- |
| License | MIT (plus MiniSat's MIT licence under `src/minisat`) | Compatible with our MIT; needs `LICENSE.note` + upstream files kept |
| Language | C++20 (`CMAKE_CXX_STANDARD 20`) | `CXX_STD = CXX20` in Makevars; oldest supported R/toolchain is a real risk |
| Hard deps | GMP (`find_package(GMP REQUIRED)`), Threads (pthreads) | System dependency → `configure` + `SystemRequirements`; the main portability risk |
| Build system | CMake ≥ 3.14 | Not usable at install time; we generate an explicit object list instead |
| Parser | flex `.ll` + bison `.yy`, **not** pre-generated in the repo | Must pre-generate at vendoring time and commit the output |
| Size | 97 `.cc`, 146 `.h`, ~2.4 MB under `src/` | ~90 TUs after pruning; install time and check time matter for CRAN |
| Include style | `<subdir/Header.h>` from source root, `"Sibling.h"` otherwise | A single `-I<vendor root>` is enough; no per-directory include flags |
| Prunable | `src/parallel`, `src/bin`, `test/`, `benchmark*/`, `docker/`, `scripts/` | Drops sockets/`signal.h` usage and the CLI executable |
| R-hostile code | `exit(`/`abort(` in `Simplex.cc`, `Interpret.{h,cc}`, `DimacsParser.h`, `FastRational.h`; ~36 files writing to `std::cout`/`printf` | Must be patched — `R CMD check` flags both |
| Entry API | `src/api/`: `MainSolver`, `Logic`/`ArithLogic`, `SMTConfig`, `PTRef`, `Interpret` | Bind `MainSolver`, not the SMT-LIB interpreter, for the R API |

## Stage 0 — Decide scope and fix package identity

Do this before writing any build glue; the answers change every later stage.

- Fill in `DESCRIPTION`: Title, Description, `Authors@R` (add OpenSMT authors as `ctb`/`cph`).
- Decide the **API surface**: (a) SMT-LIB string in / result out via `Interpret`, (b) a term-building
  API over `Logic`/`MainSolver`, or (c) both. Recommendation: (a) first — smallest binding, fastest
  to a working package — then (b) once the build is stable.
- Decide **GMP strategy**: system GMP everywhere (recommended; matches CRAN's `gmp` package) vs.
  bundling. Bundling GMP is out of scope — it is LGPL and autotools-built.
- Pin the upstream version: a release tag, not `master`.

Exit: `DESCRIPTION` is real, the API decision is written into [CLAUDE.md](CLAUDE.md).

## Stage 1 — Toolchain feasibility spike (no OpenSMT yet) — **done** (PR #1)

The single highest-risk stage. Prove C++20 + GMP + pthreads builds under `R CMD INSTALL` on every
CI configuration *before* importing 90 source files. Built:

- `tools/gmp.sh`, sourced by `configure` and `configure.win`: probes in order — caller's
  `GMP_CFLAGS`/`GMP_LIBS`, `pkg-config`, the compiler's default search path, then common prefixes
  (Homebrew first). Each probe compiles and links one program using `mpq_class` on a `std::thread`
  under a C++20 concept, so a probe passes only if all three requirements hold together. `gmpxx` is
  genuinely required, not belt-and-braces: OpenSMT's `Number` *is* `mpq_class`.
- `src/toolchain.cc` — `smt_toolchain()`, reporting compiler, `__cplusplus`, GMP runtime version and
  a rational round trip. It also establishes the exception firewall Stage 5 formalises: no object
  with a destructor is alive when `Rf_error()` longjmps.
- `SystemRequirements: GMP (>= 5.0.0), C++20`, so pak installs `libgmp-dev` on the Linux legs.

### What the first CI run established (PR #1)

1. **Windows/Rtools supplies both `libgmp` and `libgmpxx`**, on the compiler's default search path —
   no `-I`, no `-L`, no download in `configure.win`. This was the question that could have stopped
   the vendoring outright, and it is answered: green.
2. **macOS needs GMP's headers included as *system* headers.** The leg failed, and not on our code:
   gmpxx 6.3.0 declares `operator "" _mpz` with a space, which clang 18+ deprecates
   (`-Wdeprecated-literal-operator`). `R CMD check` promotes install-time compiler output to
   "significant warnings", so three warnings from a header we do not own failed the leg. Linux never
   sees it because `/usr/include` is already a system path; Homebrew's prefix is not. `tools/gmp.sh`
   now converts discovered `-I<dir>` to `-isystem <dir>`, with a `-I` fallback. This would have been
   ~90x worse after vendoring, once every OpenSMT translation unit includes `gmpxx.h`.
3. **The r-devel containers pass**: clang 23 and GCC 16 both build C++20 with GMP, as do ubuntu
   release and oldrel-1.

How GMP was actually found, per platform:

| Leg | Compiler | GMP discovered via |
| --- | --- | --- |
| windows-latest | Rtools GCC | compiler's default search path — Rtools ships `libgmpxx` and `libgmp` |
| macos-latest | Apple clang | Homebrew prefix, included with `-isystem` |
| ubuntu release / oldrel-1 | GCC | `libgmp-dev`, installed by pak from `SystemRequirements` |
| clang23 | `clang++-23 -stdlib=libc++` | pkg-config gmpxx 6.3.0 |
| ubuntu-clang / ubuntu-gcc16 | clang 22 / GCC 16 | system paths |

Note that `sysreqs` resolution worked from `SystemRequirements: GMP (>= 5.0.0), C++20` alone — pak
installed `libgmp-dev` on the Linux legs with no workflow change.

**Reading the build log in CI**: `rcmdcheck` prints the install log only on failure, so
`configure`'s output is invisible on a green run. The container legs upload a `check-*` artifact
containing `zusmt.Rcheck/00install.out`, which has it; the runner legs upload only when they fail.
For Stage 3, where compiler output is the thing being debugged, that is where to look:
`gh run download <id> -n check-clang23`.

Exit: **met** — green on all seven legs.

### CI (adopted: [pedrobtz/r-actions](https://github.com/pedrobtz/r-actions))

`.github/workflows/R-CMD-check.yaml` calls `r-actions/.github/workflows/r-cmd-check.yml@v1`: four
runner legs plus the three CRAN-like r-devel containers, each `--as-cran`, failing on a WARNING.
The container legs are the reason to have adopted it here — a package compiling ~90 files of
third-party C++ is exactly the case where CRAN's compilers differ from any GitHub runner, and that
gap shows up as someone else's warnings in the check log.

`--as-cran` enables the bashisms test, which reports a WARNING when `checkbashisms` is merely
*absent* — so the workflow sets `_R_CHECK_BASHISMS_=false` and a `configure-scripts` job runs the
real `checkbashisms --posix` and `shellcheck -s sh` on a machine that has both. Suppressing the
in-check test and then not testing it would be the wrong half of that trade.

Workflows to adopt at their stage, not before:

| Workflow | Stage | Why it waits |
| --- | --- | --- |
| `vendor.yml` | 2 | Needs `src/opensmt/` and a checksum manifest to guard |
| `vendor-upstream.yml` | 2 | Needs a pinned version for it to compare against |
| `sanitizers`, `valgrind`, `gctorture`, `rchk`, `lto` (`native-checks.yml`) | 5–7 | Want a real R/C++ boundary and a test suite to exercise it |
| `coverage.yml` (`native: true`) | 7 | The gcov table answers "how much of the vendored solver do our tests reach?" |

Exit: green on macOS, Windows, ubuntu release/oldrel-1 and the three r-devel containers.

## Stage 2 — Import the sources — **done** (PR #2)

- Write `tools/vendor.sh` (checked in, idempotent): clone at the pinned tag → copy `src/` → delete
  `parallel/`, `bin/` → run `bison`/`flex` on `smt2newparser.yy` / `smt2newlexer.ll` and place the
  generated `.cc`/`.hh` next to them → write `src/opensmt/VERSION` with tag + commit SHA + date.
  flex/bison run on the *maintainer's* machine only; the generated files are committed so that
  neither is needed at install time.
- Copy upstream `LICENSE` and `src/minisat/LICENSE` into `LICENSE.note`; list OpenSMT copyright
  holders in `DESCRIPTION` as `cph`.
- Keep the vendored tree byte-identical to upstream apart from Stage 4's patch series.

Adopt `vendor.yml` and `vendor-upstream.yml` in the same commit: the first fails a PR that edits a
vendored file without updating the manifest and checksums, the second opens an issue when upstream
ships a release past our pin.

What landed: **OpenSMT v2.9.2** (`34bc1b8`, 2025-06-16), 238 files under `src/opensmt/`. v2.9.2
rather than `master` because `vendor-upstream.yml` compares the pin against upstream's latest
*release*, so pinning a moving branch makes that check meaningless; its `CMakeLists.txt` asks for the
same C++20, GMP and Threads that Stage 1 proved, so that result carries over.

Removed at import: `parallel/` (sockets and signals), `bin/` (the CLI executable), and every
`CMakeLists.txt` — a build description in-tree for a build this package does not use is a trap, not
documentation. The SMT-LIB parser is generated here with bison 3.8.2 and flex 2.6.4 and committed,
because neither tool can be an install-time dependency.

Exit: **met** — `tools/vendor.sh` run twice produces byte-identical checksums, and
`tools/vendor/verify` passes against the manifest.

## Stage 3 — Build it with R's toolchain — **done** (PR #3, with Stage 4)

- Generate `src/Makevars` object list from the vendored tree (a `tools/` script emitting an explicit
  `OBJECTS = opensmt/api/MainSolver.o ...`). Explicit list, not `$(wildcard)` — that would force
  `SystemRequirements: GNU make`.
- `PKG_CPPFLAGS = -I. -Iopensmt @GMP_CFLAGS@`, `PKG_LIBS = @GMP_LIBS@`; `Makevars.win` mirrors it.
- Expect to fix: unused-parameter/sign-compare warning noise, missing `#include` under newer libstdc++,
  `-Wall -Wextra` diagnostics CRAN surfaces. Do not add `-w`; CRAN rejects flag suppression.
- Watch install time and `.so` size; if unacceptable, revisit pruning (interpolation, proofs,
  lookahead and unsat cores are candidates).

**Stages 3 and 4 turned out to be one stage.** Stage 3 alone compiles and links, but
`R CMD check --as-cran` then reports a compiled-code WARNING for upstream's `std::cerr`, `printf`,
`rand`/`srand` and `exit`/`abort`, and the workflow fails on a WARNING — so a Stage 3 PR could not be
green, and the two shipped together.

Two findings worth keeping:

- **The object list must come from upstream's CMakeLists, not from `find`.** v2.9.2 ships
  `tsolvers/bvsolver/` and `logics/BVLogic.cc` but comments both out of its build, and they do not
  compile (`BVLogic` is left incomplete). 87 sources build, of 91 present. This also reverses Stage
  2's decision to delete the `CMakeLists.txt` files as "a trap": they are the only authoritative
  statement of what upstream compiles, and `tools/objects.sh` now reads them.
- **R compiles the vendored tree in place**, so a build leaves `.o` files inside `src/opensmt/` and
  the checksum manifest has to exclude them, or `verify` fails on any built tree.

Exit: **met** — 87 sources compile, and `smoke_solve()` returns `unsat` for `a AND NOT a` and `sat`
for `a`, from R, through `MainSolver`.

## Stage 4 — Patch upstream for R hosting — **done** (PR #3, with Stage 3)

Maintain these as a numbered patch series under `tools/patches/`, applied by `vendor.sh`, so a version
bump is re-applicable rather than re-discovered.

- `exit()`/`abort()` → throw (`ApiException`/`InternalException` already exist upstream).
- `std::cout`/`printf` → `Rprintf`/`REprintf`, or route through an output sink the R layer controls.
- `std::cerr` diagnostics → `REprintf`.
- Check for `srand`/`rand` and RNG seeding that should defer to R, and for any `assert` that aborts in
  release builds.
- Long-running solves must remain interruptible: plan for `R_CheckUserInterrupt()` via OpenSMT's stop
  callback (`GlobalStop`), never a longjmp through C++ frames.

What the rules are, in `tools/patches.sh`, each asserting it changed something so that a bump which
makes one a no-op fails loudly rather than silently leaving a call site behind:

| Rule | Scale |
| --- | --- |
| `std::cout`/`std::cerr` → `zusmt::rout()`/`rerr()`, ostreams over `Rprintf`/`REprintf` | 14 + 20 files |
| `printf` → `Rprintf`; `fprintf(stderr,` → `REprintf(` | 19 + 7 files |
| `rand`/`srand` → a self-contained xorshift | 2 files |
| `exit`/`abort` → `zusmt::fatal()`, which throws | 4 call sites, individually |
| the `stdout`/`stderr` `FILE*` symbols themselves | 3 files |

The last two are per-call-site rather than blanket rewrites, because `Interpret` has a *method* named
`exit()` and 4 `fprintf` calls write to a real file rather than the console — a blanket rule would
either not compile or send proof output to the console.

Windows took five CI round-trips, each a different assumption: no `<sys/resource.h>`, no
`suseconds_t`, a 32-bit `long`, then `dprintf`/`asprintf` and a `size_t` signature mismatch.
A sixth was a *link* failure, not a compile one: upstream's `inline static thread_local mpz_class
temp` compiles everywhere and links nowhere on mingw, which emits the TLS init wrapper without
COMDAT linkage so all 28 translation units touching `FastRational` define it.
`tools/check-mingw.sh` covers both classes locally — compile by default, `--link` to link the
objects with unresolved symbols ignored purely to find duplicate definitions. Use it after a bump,
before pushing.

The portability problems CI found, all now rules in the same script:

- **Windows has no `<sys/resource.h>`.** Upstream times itself with `getrusage()`, so the mingw build
  failed at the first file including `Timer.h`. [src/r_rusage.h](src/r_rusage.h) supplies it on
  Windows via `clock()` — not `GetProcessTimes()`, because `<windows.h>` would reach 87 translation
  units and bring its `min`/`max` and `ERROR` macros with them. The two `<sys/wait.h>` includes were
  vestigial (neither file calls `fork` or `wait`) and are simply dropped.
- **libgmpxx is an ABI dependency, and was only needed for one function.** `operator<<(ostream&,
  mpq)` lives in the library and is compiled against whichever C++ standard library built it, so on
  a libc++ toolchain linking a libstdc++-built libgmpxx — CRAN's `clang23` container, FreeBSD, any
  mixed setup — the shared object fails to load with an undefined symbol. It was the *only* C++
  symbol we took from libgmpxx; everything else is plain C. Two call sites now use header-only
  `get_str()` instead, which drops the dependency rather than working around it.

Exit: **met** — `R CMD check --as-cran` reports no compiled-code findings.

## Stage 5 — The R/C++ boundary — **done** (PR #4)

- Solver handles as external pointers with finalizers; `R_RegisterCFinalizerEx(..., TRUE)`.
- One exception firewall macro wrapping every `.Call` entry: `catch (std::exception&)` →
  `Rf_error()`; never let an exception or a `longjmp` cross the other's frames.
- `R_init_zusmt` with `R_registerRoutines` + `R_useDynamicSymbols(dll, FALSE)`
  (`@useDynLib zusmt, .registration = TRUE` is already in [R/zusmt-package.R](R/zusmt-package.R)).
- Keep the boundary file(s) small and in `src/` top level, separate from `src/opensmt/`.

How interruption works, since it was the part with no obvious answer in v2.9.2 (`GlobalStop` is a
`master` thing and does not exist at this pin): `CoreSMTSolver::okContinue()` is the search loop's
own termination check, and a patch rule makes it also ask `zusmt::interrupt_requested()`. A pending
interrupt therefore ends the search exactly the way upstream's own stop flag does — by unwinding
normally through every destructor — and `check()` returns `s_Undef`. The boundary raises the
interrupt in R *after* returning from the firewall, which is the first point at which no C++ frame
is left for the longjmp to skip.

Detecting a pending interrupt without delivering it is `R_ToplevelExec(R_CheckUserInterrupt)`, whose
longjmp lands in that fresh context rather than in our frames. Polls are throttled to 50 ms, since
`okContinue()` is called from the innermost loop.

Measured: 10 pigeons into 9 holes takes ~18 s to prove unsat; with an interrupt armed three polls
in, the same solve returns in under a second.

Exit: **met** — 26 tests, covering `gc()` survival, collection of unreferenced handles, explicit
release being idempotent rather than a double free, foreign external pointers rejected by tag, and a
C++ exception arriving as an ordinary R condition with the session still usable.

## Stage 6 — R-facing API

Per the Stage 0 decision. A plausible minimum: `smt_solver()`, `smt_assert()`, `smt_check()`,
`smt_model()`, plus `print`/`format` methods. Decide error vs. value semantics for `unknown`, and how
rationals come back to R (double, character, or `gmp::bigq`).

Exit: the README example is real code that runs.

## Stage 7 — Tests and memory hygiene

- `testthat` unit tests per logic (QF_UF, QF_LRA, QF_LIA, QF_AX), plus error paths and interrupts.
- Regression corpus of small SMT-LIB files under `inst/` (keep the package tarball small).
- ASAN/UBSAN and valgrind runs via `rhub::rhub_check()`; add a `--use-valgrind` note to CLAUDE.md.
- Guard check time: keep examples and tests fast; CRAN's limit is the practical constraint.

Exit: clean sanitizer runs; tests meaningfully exercise the boundary, not just the happy path.

## Stage 8 — Documentation

- Roxygen for every exported function, runnable `@examples`.
- README rewritten around a real example; `pkgdown` reference index in `_pkgdown.yml`.
- A vignette showing an end-to-end problem, and a `NEWS.md` entry recording the vendored OpenSMT
  version.
- Document the vendoring process itself (how to bump OpenSMT) — `.github/CONTRIBUTING.md` or CLAUDE.md.

## Stage 9 — CRAN submission

- Run the `cran-extrachecks` skill.
- Verify: `SystemRequirements` accurate, `LICENSE.note` present, copyright holders credited,
  install time and tarball size acceptable, no compiler warnings on CRAN's flavours, `--as-cran`
  clean including the "libs size" note.
- Expect a reviewer question about the bundled third-party sources; have the provenance answer ready.

## Standing concerns

- **Version bumps**: `vendor.sh` + patch series is the whole maintenance story. If a patch stops
  applying, that is the signal upstream changed something we depend on.
- **C++20 on oldrel**: if oldrel-1 ever fails to compile the vendored tree, the choice is dropping the
  platform from the matrix or `Requires: R (>= x.y)`, not downgrading upstream.
- **Windows GMP** is the most likely blocker; Stage 1 exists to find it early.
