; Recursive factorial
(define factorial
    (lambda (n)
        (if (<= n 1)
            1
            (* n (factorial (- n 1)))
        )
    )
)

(print (factorial 6) "\n") ; Should be 720

; Recursive Fibonacci
(define fib
    (lambda (n)
        (if (< n 2)
            n
            (+ (fib (- n 1)) (fib (- n 2)))
        )
    )
)

(print (fib 8) "\n") ; Should be 21

