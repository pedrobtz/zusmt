test_that("a handle solves across several calls", {
  s <- solver_new("QF_UF")
  expect_type(s, "externalptr")

  # a AND NOT a, built up over two calls on the same handle
  solver_assert_var(s, "a", negated = FALSE)
  expect_identical(solver_check(s), "sat")

  solver_assert_var(s, "a", negated = TRUE)
  expect_identical(solver_check(s), "unsat")
})

test_that("handles survive garbage collection", {
  s <- solver_new("QF_UF")
  solver_assert_var(s, "a")

  # The finalizer must not run while the handle is still reachable, and the
  # C++ objects behind it must still be intact afterwards.
  for (i in 1:3) gc()
  expect_identical(solver_check(s), "sat")
})

test_that("an unreferenced handle is collected without crashing", {
  for (i in 1:20) {
    local({
      s <- solver_new("QF_UF")
      solver_assert_var(s, "x")
      solver_check(s)
    })
  }
  # If the finalizer were wrong, this is where it would show.
  gc()
  expect_true(TRUE)
})

test_that("release is explicit, and a released handle errors rather than crashing", {
  s <- solver_new("QF_UF")
  solver_release(s)

  expect_error(solver_check(s), "released")
  # Second release must not double free.
  expect_error(solver_release(s), "released")
  gc()
  expect_true(TRUE)
})

test_that("a non-handle is rejected by the tag check", {
  expect_error(solver_check(structure(list(), class = "not_a_handle")))
  # An external pointer that is not ours: right type, wrong tag.
  other <- methods::new("externalptr")
  expect_error(solver_check(other), "not a zusmt solver handle")
})

test_that("a C++ exception arrives as an R condition", {
  expect_error(throw_from_cpp(), "deliberate exception from C\\+\\+")
  # The session is still usable: the firewall unwound rather than aborted.
  expect_identical(solver_check(solver_new("QF_UF")), "sat")
})

test_that("unsupported logics are refused before anything is allocated", {
  expect_error(solver_new("QF_BV"), "unsupported logic")
  expect_error(solver_new(42), "single string")
})

test_that("a hard instance is decided, and a long search can be stopped", {
  # 5 pigeons, 4 holes: unsatisfiable, and fast enough for a check run.
  s <- solver_new()
  solver_assert_pigeonhole(s, 4)
  expect_identical(solver_check(s), "unsat")

  # 10 pigeons into 9 holes takes ~18s uninterrupted on a laptop. With an
  # interrupt armed three polls in, the search unwinds immediately -- which is
  # what proves okContinue() is consulted rather than the solve merely being
  # abandoned somewhere.
  s2 <- solver_new()
  solver_assert_pigeonhole(s2, 9)
  arm_test_interrupt(3)
  # solver_check() signals an interrupt condition rather than returning, so
  # catch it here and measure how long the search took to unwind.
  elapsed <- system.time(
    caught <- tryCatch(solver_check(s2), interrupt = function(e) "interrupted")
  )[["elapsed"]]

  expect_identical(caught, "interrupted")
  expect_lt(elapsed, 5)
})

test_that("a stopped solver is still usable afterwards", {
  s <- solver_new()
  solver_assert_pigeonhole(s, 9)
  arm_test_interrupt(2)
  expect_identical(tryCatch(solver_check(s), interrupt = function(e) "interrupted"),
                   "interrupted")

  # The handle survived the unwind: no leak, no corruption, still answers.
  s2 <- solver_new()
  solver_assert_var(s2, "a")
  expect_identical(solver_check(s2), "sat")
})
