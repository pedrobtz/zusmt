; logic: QF_IDL
; expect: unsat
; A negative cycle in difference logic: x < y < z < x.
(declare-const x Int)
(declare-const y Int)
(declare-const z Int)
(assert (< x y))
(assert (< y z))
(assert (< z x))
