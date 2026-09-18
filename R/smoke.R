# Stage 3 scaffolding: not exported, and replaced by the real API at Stage 6.
# It exists so the test suite can prove the vendored solver runs in-process.
smoke_solve <- function(contradiction = TRUE) {
  .Call(C_smoke_solve, contradiction)
}
