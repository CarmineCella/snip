;; Examples for the Snip standard library

(load "stdlib.scm")

;; --- map ---
(map (lambda (x) (+ x 1)) (list 1 2 3))
; => (2 3 4)

;; --- fold ---
(fold + 0 (list 1 2 3 4))
; => 10

;; --- filter ---
(filter (lambda (x) (eq? (/ x 2) 2)) (list 4 3 3 4))
; => (4 4)

;; --- element ---
(element 3 (list 1 2 3 4))
; => 1

;; --- reverse ---
(reverse (list 1 2 3))
; => (3 2 1)

;; --- append ---
(append (list 1 2) (list 3 4))
; => (1 2 3 4)

;; --- length ---
(length (list 5 6 7))
; => 3

;; --- assoc ---
(assoc 'a (list (list 'a 1) (list 'b 2)))
; => (a 1)

;; --- memq ---
(memq 'b (list 'a 'b 'c))
; => (b c)

;; --- remove ---
(remove 2 (list 1 2 3 2 4))
; => (1 3 4)

;; --- find ---
(find (lambda (x) (eq? x 3)) (list 1 2 3 4))
; => 3

;; --- forall ---
(forall (lambda (x) (eq? (/ x 2) 2)) (list 4 4 4))
; => 1

;; --- exists ---
(exists (lambda (x) (eq? x 3)) (list 1 2 4))
; => 0

;; --- last ---
(last (list 1 2 3 4))
; => 4

;; --- take ---
(take 2 (list 1 2 3 4))
; => (1 2)

;; --- drop ---
(drop 2 (list 1 2 3 4))
; => (3 4)

;; --- flatten ---
(flatten (list 1 (list 2 (list 3 4)) 5))
; => (1 2 3 4 5)

;; --- zip ---
(zip (list 1 2 3) (list 'a 'b 'c))
; => ((1 . a) (2 . b) (3 . c))

;; eof

