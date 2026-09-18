; logic: QF_UF
; expect: unsat
; Congruence: equal arguments must give equal results, so asserting
; otherwise is unsatisfiable. This exercises the egraph rather than the
; SAT core.
(declare-sort U 0)
(declare-fun f (U) U)
(declare-const x U)
(declare-const y U)
(assert (= x y))
(assert (distinct (f x) (f y)))
