; logic: QF_AX
; expect: unsat
; Reading back what was just written must give the written value.
(declare-sort I 0)
(declare-sort E 0)
(declare-const arr (Array I E))
(declare-const i I)
(declare-const e E)
(assert (distinct (select (store arr i e) i) e))
