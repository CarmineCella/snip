;; If expressions

(define test_expr (lambda (x)
    (if (< x 10)
        "small\n"
        "big\n"
    )
))


(display (test_expr 3))
(display (test_expr 20))

;; eof

