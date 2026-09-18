; logic: QF_LRA
; expect: sat
; The solution is 1/3, which is not representable as a double -- the reason
; model values carry an exact form.
(declare-const r Real)
(assert (= (* 3.0 r) 1.0))
