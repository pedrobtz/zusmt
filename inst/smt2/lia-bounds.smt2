; logic: QF_LIA
; expect: sat
(declare-const x Int)
(declare-const y Int)
(assert (> x 3))
(assert (< x 7))
(assert (= y (* 2 x)))
