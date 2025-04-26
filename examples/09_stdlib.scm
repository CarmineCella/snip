;; Load the standard library
(load "stdlib.scm")

(print "\n=== List Manipulation Demo ===\n")

;; Create some lists
(define nums (list 1 2 3 4 5 6))
(define words (list "apple" "banana" "cherry"))

(print "Original nums: " nums "\n")
(print "Original words: " words "\n")

;; --- Map ---
(print "\n-- Map square function over nums --\n")
(define square (lambda (x) (* x x)))
(print (map square nums) "\n")  ; (1 4 9 16 25 36)

;; --- Filter ---
(print "\n-- Filter even numbers --\n")
(print (filter (lambda (x) (eq? (/ x 2) 2)) (list 4 3 3 4)) "\n") ; (4 4)

;; --- Reverse ---
(print "\n-- Reverse nums --\n")
(print (reverse nums) "\n") ; (6 5 4 3 2 1)

;; --- Fold ---
(print "\n-- Fold sum of nums --\n")
(print (fold + 0 nums) "\n") ; 21

;; --- Flatten ---
(print "\n-- Flatten nested lists --\n")
(define nested (list (list 1 2) (list 3 (list 4 5)) 6))
(print nested "\n")
(print (flatten nested) "\n") ; (1 2 3 4 5 6)

;; --- Zip ---
(print "\n-- Zip nums and words --\n")
(print (zip nums words) "\n")
; ((1 . "apple") (2 . "banana") (3 . "cherry"))

;; --- Find ---
(print "\n-- Find first >3 in nums --\n")
(define greater-than-3 (lambda (x) (> x 3)))
(print (find greater-than-3 nums) "\n") ; 4

;; --- Forall / Exists ---
(print "\n-- Forall and Exists --\n")
(print "All >0: " (forall (lambda (x) (> x 0)) nums) "\n") ; 1
(print "Exists >5: " (exists (lambda (x) (> x 5)) nums) "\n") ; 1
(print "Exists >10: " (exists (lambda (x) (> x 10)) nums) "\n") ; 0

;; --- Last ---
(print "\n-- Last element --\n")
(print (last nums) "\n") ; 4

;; --- Take ---
(print "\n-- Take first 2 elements --\n")
(print (take 2 nums) "\n") ; (1 2)

;; --- Drop ---
(print "\n-- Drop first 2 elements --\n")
(print (drop 2 nums) "\n") ; (3 4)

(print "\n=== Tree Example ===\n")

;; Build a tree (nested lists)
(define tree (list 1 (list 2 (list 3 4)) 5 (list 6 7)))

(print "\n-- Original Tree --\n")
(print tree "\n")

;; Flatten the tree
(print "\n-- Flatten Tree --\n")
(define flat-tree (flatten tree))
(print flat-tree "\n") ; (1 2 3 4 5 6 7)

;; Map over the tree (square each element)
(print "\n-- Map (square) over flattened tree --\n")
(define square (lambda (x) (* x x)))
(print (map square flat-tree) "\n") ; (1 4 9 16 25 36 49)

;; Sum all elements of tree
(print "\n-- Sum of elements --\n")
(print (fold + 0 flat-tree) "\n") ; 28

;; Find if 5 exists
(print "\n-- Find if 5 exists --\n")
(print (find (lambda (x) (eq? x 5)) flat-tree) "\n") ; 5

;; Check forall > 0
(print "\n-- Forall elements >0 --\n")
(print (forall (lambda (x) (> x 0)) flat-tree) "\n") ; 1

;; Check exists element >10
(print "\n-- Exists element >10 --\n")
(print (exists (lambda (x) (> x 10)) flat-tree) "\n") ; 0

(print "\n=== Demo Completed ===\n")
