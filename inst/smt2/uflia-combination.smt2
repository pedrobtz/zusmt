; logic: QF_UFLIA
; expect: unsat
; Combines an uninterpreted function with integer arithmetic: both theories
; must agree for this to be decided.
(declare-fun f (Int) Int)
(declare-const a Int)
(assert (= (f a) 10))
(assert (= a 1))
(assert (distinct (f 1) 10))
