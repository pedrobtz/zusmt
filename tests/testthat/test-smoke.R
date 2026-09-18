test_that("the vendored solver decides two tiny QF_UF problems", {
  # "a AND NOT a" cannot be satisfied; "a" on its own can.
  expect_identical(smoke_solve(contradiction = TRUE), "unsat")
  expect_identical(smoke_solve(contradiction = FALSE), "sat")
})
