; logic: QF_IDL
; expect: unsupported
; Not difference logic: x + y < z. QF_IDL's solver used to misread the sum
; and answer "sat" with x = 1, y = 1, z = 0; it is now refused (#29).
(declare-const x Int)
(declare-const y Int)
(declare-const z Int)
(assert (< (+ x y) z))
(assert (> x 0))
(assert (> y 0))
(assert (< z 2))
