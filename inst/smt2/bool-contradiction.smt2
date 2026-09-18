; logic: QF_UF
; expect: unsat
; The smallest contradiction there is.
(declare-const a Bool)
(assert a)
(assert (not a))
