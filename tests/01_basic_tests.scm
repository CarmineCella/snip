;; Basic tests

(load "stdlib.scm")

;; --- Arithmetic ---
(test (+ 1 2 3) 6)
(test (- 10 3 2) 5)
(test (* 2 3 4) 24)
(test (/ 100 2 5) 10)

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

;; --- Conditionals ---
(define check (lambda (n) (if (< n 10) 1 0)))
(test (check 5) 1)
(test (check 15) 0)

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

;; --- Types ---
(test (type 1) number )
(test (type 'a) symbol)
(test (type "hallo") string)
(test (type (lambda (x)(+ x x))) lambda)
(test (type (list)) list)
(test (type ()) list)
(test (type +) op)

;; --- Equalities ---
(test (eq? 1 1) 1)
(test (eq? 1 0) 0)
(test (eq? 'a 'a) 1)
(test (eq? 'a 'b) 0)
(test (eq? (list 1 2 3) (list 1 2 3)) 1)
(define b (list 2 3))
(test (eq? b b) 1)
(test (eq? (list 1 2 3) b) 0)
(test (eq? + +) 1)
(test (eq? square square) 1)
(test (eq? square add) 0)
(test (eq? "hallo" "hallo") 1)
(test (eq? "hallo" "world") 0)

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

(display "\n--- Tests completed ---\n")

;;  eof

