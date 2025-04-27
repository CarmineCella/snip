;; While loop

(define counter 0)
(define total 0)

(while (< counter 5)
    (begin
        (display "iteration " counter "\n")
        (set! total (+ total counter))
        (set! counter (+ counter 1))
    )
)

(display "total: "  total "\n")

;; eof

