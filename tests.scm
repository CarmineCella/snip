(load "stdlib.scm")

(define test-pass 0)
(define test-fail 0)

(define test
  (lambda (expr expected)
    (begin
      (define expr-str expr)
      (define result (eval expr-str))
      (if (eq? result expected)
          (begin
            (print "PASS: " expr-str " => " result "\n")
            (set! test-pass (+ test-pass 1))
          )
          (begin
            (print "FAIL: " expr-str " => got " result " expected " expected "\n")
            (set! test-fail (+ test-fail 1))
          )
      )
    )
  )
)

(define report
  (lambda () (begin
    (print "\n=== TEST REPORT ===\n")
    (print "Passed: " test-pass "\n")
    (print "Failed: " test-fail "\n")
    (if (eq? test-fail 0)
        (print "ALL TESTS PASSED\n")
        (print "Some tests FAILED\n")
    )
  ))
)

(print "=== Running Tests ===\n")


;; --- Arithmetic ---
(test (+ 1 2 3) 6)
(test (- 10 3 2) 5)
(test (* 2 3 4) 24)
(test (/ 100 2 5) 10)

(test (fold + 0 (list 1 2 3 4)) 10)

;; --- Variables ---
(define x 10)
(define y 20)
(test (+ x y) 30)

(set! x 15)
(test (+ x y) 35)

;; --- Functions ---
(define add (lambda (a b) (+ a b)))
(test (add 5 7) 12)

(define square (lambda (x) (* x x)))
(test (square 4) 16)

;; Using map
(test (car (map square (list 2 3 4))) 4)
(test (car (cdr (map square (list 2 3 4)))) 9)

;; --- Conditionals ---
(define check (lambda (n) (if (< n 10) 1 0)))
(test (check 5) 1)
(test (check 15) 0)

;; Logical operators
(test (and 1 1) 1)
(test (and 0 1) 0)
(test (or 0 1) 1)
(test (or 0 0) 0)
(test (not 0) 1)
(test (not 1) 0)

;; --- Loops ---
(define c 0)
(define t 0)
(while (< c 5)
    (begin
        (set! t (+ t c))
        (set! c (+ c 1))
    )
)
(test t 10)

;; --- Lists ---
(define lst (list 1 2 3 4 5))
(test (car lst) 1)
(test (car (cdr lst)) 2)

(test (car (cons 0 lst)) 0)

(test (car (reverse (list 1 2 3))) 3)

(test (car (cdr (append (list 1) (list 2 3)))) 2)

;; Filtering even numbers
(define even (lambda (x) (eq? (/ x 2) 2)))
(test (car (filter even (list 4 3 3 4))) 4)

;; Length
(test (length (list 1 2 3)) 3)

;; Flatten
(test (car (flatten (list (list 1 2) (list 3 4)))) 1)

;; Zip
(define zipped (zip (list 1 2) (list "a" "b")))
(test (car (car (car zipped))) 1)

;; --- Strings ---
(test (string 'length "hello") 5)
(test (string 'find "hello world" "world") 6)
(test (string 'range "hello world" 0 5) "hello")
(test (car (string 'split "a,b,c" ",")) "a")

;; --- Recursion: factorial ---
(define factorial
  (lambda (n)
    (if (<= n 1)
        1
        (* n (factorial (- n 1)))
    )
  )
)

(test (factorial 5) 120)
(test (factorial 0) 1)

;; --- Recursion: Fibonacci ---
(define fib
  (lambda (n)
    (if (< n 2)
        n
        (+ (fib (- n 1)) (fib (- n 2)))
    )
  )
)

(test (fib 0) 0)
(test (fib 1) 1)
(test (fib 5) 5)
(test (fib 7) 13)

;; --- Final Report ---
(report)
