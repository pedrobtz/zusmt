; logic: QF_UF
; expect: sat
; A satisfiable propositional formula.
(declare-const a Bool)
(declare-const b Bool)
(assert (or a b))
(assert (not (and a b)))
