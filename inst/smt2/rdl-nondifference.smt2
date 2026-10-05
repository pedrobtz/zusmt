; logic: QF_RDL
; expect: unsupported
; Not difference logic: x + y < 1 and x + y > 2. QF_RDL's solver used to
; answer "sat" with x = 3, y = 1; it is now refused (#29).
(declare-const x Real)
(declare-const y Real)
(assert (< (+ x y) 1.0))
(assert (> x 0.0))
(assert (> y 0.0))
(assert (> (+ x y) 2.0))
