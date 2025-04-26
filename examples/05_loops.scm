; While loop
(define counter 0)
(define total 0)

(while (< counter 5)
    (begin
        (set! total (+ total counter))
        (set! counter (+ counter 1))
    )
)

(print total "\n")

