; If expressions
(define test_expr (lambda (x)
    (if (< x 10)
        "small\n"
        "big\n"
    )
))


(print (test_expr 3))
(print (test_expr 20))
