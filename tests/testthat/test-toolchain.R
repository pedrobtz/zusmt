test_that("the package is built with C++20, GMP and threads", {
  info <- smt_toolchain()

  expect_type(info, "list")
  # OpenSMT sets CMAKE_CXX_STANDARD 20 and does not build below it.
  expect_gte(info$cplusplus, 202002)
  # GMP's C++ bindings did the arithmetic, not a fallback.
  expect_identical(info$rational, "1/2")
  expect_true(nzchar(info$gmp_version))
  expect_true(info$threads_ok)
})
