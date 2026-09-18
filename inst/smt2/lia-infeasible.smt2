; logic: QF_LIA
; expect: unsat
; Integer-only: 2x = 3 has a rational solution but no integer one, so this
; distinguishes QF_LIA from QF_LRA rather than merely using integers.
(declare-const x Int)
(assert (= (* 2 x) 3))
