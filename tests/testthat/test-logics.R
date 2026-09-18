# Every logic the package advertises must actually solve, not merely be
# accepted by smt_solver(). The supported list in check_logic_supported() and
# the list documented in ?smt_solver are a promise; this is what checks it.

logic_probes <- list(
  QF_UF    = list(sat = "(declare-const a Bool)(assert a)",
                  unsat = "(declare-const a Bool)(assert a)(assert (not a))"),
  QF_LIA   = list(sat = "(declare-const x Int)(assert (> x 3))",
                  unsat = "(declare-const x Int)(assert (= (* 2 x) 3))"),
  QF_LRA   = list(sat = "(declare-const r Real)(assert (> r 0.5))",
                  unsat = "(declare-const r Real)(assert (> r 1.0))(assert (< r 0.0))"),
  QF_UFLIA = list(sat = "(declare-fun f (Int) Int)(declare-const x Int)(assert (= (f x) 3))",
                  unsat = paste("(declare-fun f (Int) Int)(declare-const a Int)",
                                "(assert (= a 1))(assert (= (f a) 10))(assert (distinct (f 1) 10))")),
  QF_UFLRA = list(sat = "(declare-fun g (Real) Real)(declare-const r Real)(assert (= (g r) 1.5))",
                  unsat = paste("(declare-fun g (Real) Real)(declare-const a Real)",
                                "(assert (= a 1.0))(assert (= (g a) 2.0))(assert (distinct (g 1.0) 2.0))")),
  QF_IDL   = list(sat = "(declare-const p Int)(declare-const q Int)(assert (<= (- p q) 3))",
                  unsat = paste("(declare-const x Int)(declare-const y Int)(declare-const z Int)",
                                "(assert (< x y))(assert (< y z))(assert (< z x))")),
  QF_RDL   = list(sat = "(declare-const u Real)(declare-const v Real)(assert (<= (- u v) 3.0))",
                  unsat = paste("(declare-const u Real)(declare-const v Real)",
                                "(assert (< u v))(assert (< v u))")),
  QF_AX    = list(sat = paste("(declare-sort I 0)(declare-sort E 0)",
                              "(declare-const arr (Array I E))(declare-const i I)(declare-const e E)",
                              "(assert (= (select (store arr i e) i) e))"),
                  unsat = paste("(declare-sort I 0)(declare-sort E 0)",
                                "(declare-const arr (Array I E))(declare-const i I)(declare-const e E)",
                                "(assert (distinct (select (store arr i e) i) e))"))
)

test_that("every advertised logic decides both a sat and an unsat problem", {
  for (logic in names(logic_probes)) {
    probes <- logic_probes[[logic]]

    s <- smt_solver(logic)
    smt_assert(s, probes$sat)
    expect_identical(smt_check(s), "sat", info = logic)

    u <- smt_solver(logic)
    smt_assert(u, probes$unsat)
    expect_identical(smt_check(u), "unsat", info = logic)
  }
})

test_that("every logic the package supports has a probe here", {
  # smt_logics() reads the C++ array that check_logic_supported() gates on, so
  # this compares the probes against the package's own answer rather than
  # against a second copy of the list in this file. Add a logic to that array
  # and this fails until it has a sat and an unsat probe.
  expect_setequal(names(logic_probes), smt_logics())

  # And each is genuinely accepted -- a weaker claim, but it catches a typo in
  # the array that the probes would otherwise mask.
  for (logic in smt_logics()) {
    expect_s3_class(smt_solver(logic), "zusmt_solver")
  }
})
