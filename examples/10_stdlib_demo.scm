;; Standard Library Demo

;; Load the standard library
(load "stdlib.scm")

(display "\n=== Standard Library Demo ===\n")

;; Create some lists
(define nums (list 1 2 3 4 5 6))
(define words (list "apple" "banana" "cherry"))

(display "Original nums: " nums "\n")
(display "Original words: " words "\n")

;; --- Map ---
(display "\n-- Map square function over nums --\n")
(define square (lambda (x) (* x x)))
(display (map square nums) "\n")  ; (1 4 9 16 25 36)

;; --- Iterate ---
(display "\n-- Iterate function --\n")
(display (iterate (lambda (x) (+ x 1)) 1 5) "\n")

;; --- Filter ---
(display "\n-- Filter even numbers --\n")
(display (filter (lambda (x) (eq? (/ x 2) 2)) (list 4 3 3 4)) "\n") ; (4 4)

;; --- Reverse ---
(display "\n-- Reverse nums --\n")
(display (reverse nums) "\n") ; (6 5 4 3 2 1)

;; --- Fold ---
(display "\n-- Fold sum of nums --\n")
(display (fold + 0 nums) "\n") ; 21

;; --- Flatten ---
(display "\n-- Flatten nested lists --\n")
(define nested (list (list 1 2) (list 3 (list 4 5)) 6))
(display nested "\n")
(display (flatten nested) "\n") ; (1 2 3 4 5 6)

;; --- Zip ---
(display "\n-- Zip nums and words --\n")
(display (zip nums words) "\n")
; ((1 . "apple") (2 . "banana") (3 . "cherry"))

;; --- Find ---
(display "\n-- Find first >3 in nums --\n")
(define greater-than-3 (lambda (x) (> x 3)))
(display (find greater-than-3 nums) "\n") ; 4

;; --- Forall / Exists ---
(display "\n-- Forall and Exists --\n")
(display "All >0: " (forall (lambda (x) (> x 0)) nums) "\n") ; 1
(display "Exists >5: " (exists (lambda (x) (> x 5)) nums) "\n") ; 1
(display "Exists >10: " (exists (lambda (x) (> x 10)) nums) "\n") ; 0

;; --- Last ---
(display "\n-- Last element --\n")
(display (last nums) "\n") ; 4

;; --- Take ---
(display "\n-- Take first 2 elements --\n")
(display (take 2 nums) "\n") ; (1 2)

;; --- Drop ---
(display "\n-- Drop first 2 elements --\n")
(display (drop 2 nums) "\n") ; (3 4)

(display "\n=== Tree Example ===\n")

;; Build a tree (nested lists)
(define tree (list 1 (list 2 (list 3 4)) 5 (list 6 7)))

(display "\n-- Original Tree --\n")
(display tree "\n")

;; Flatten the tree
(display "\n-- Flatten Tree --\n")
(define flat-tree (flatten tree))
(display flat-tree "\n") ; (1 2 3 4 5 6 7)

;; Map over the tree (square each element)
(display "\n-- Map (square) over flattened tree --\n")
(define square (lambda (x) (* x x)))
(display (map square flat-tree) "\n") ; (1 4 9 16 25 36 49)

;; Sum all elements of tree
(display "\n-- Sum of elements --\n")
(display (fold + 0 flat-tree) "\n") ; 28

;; Find if 5 exists
(display "\n-- Find if 5 exists --\n")
(display (find (lambda (x) (eq? x 5)) flat-tree) "\n") ; 5

;; Check forall > 0
(display "\n-- Forall elements >0 --\n")
(display (forall (lambda (x) (> x 0)) flat-tree) "\n") ; 1

;; Check exists element >10
(display "\n-- Exists element >10 --\n")
(display (exists (lambda (x) (> x 10)) flat-tree) "\n") ; 0

(display "\n--- More tests ---\n")

(display "range: ") (display (range 0 10)) (display "\n")
(display "map (x+1): ") (display (map (lambda (x) (+ x 1)) (range 0 5))) (display "\n")
(display "filter (>2): ") (display (filter (lambda (x) (> x 2)) (range 0 5))) (display "\n")
(display "fold (+): ") (display (fold + 0 (range 1 6))) (display "\n")
(display "reverse: ") (display (reverse (range 0 5))) (display "\n")
(display "iterate (double): ") (display (iterate (lambda (x) (* 2 x)) 1 5)) (display "\n")
(display "append: ") (display (append (range 0 3) (range 3 6))) (display "\n")
(display "element 3: ") (display (element 3 (range 0 5))) (display "\n")
(display "length: ") (display (length (range 0 5))) (display "\n")
(display "assoc: ") (display (assoc 'b (list (list 'a 1) (list 'b 2) (list 'c 3)))) (display "\n")
(display "memq 'b: ") (display (memq 'b (list 'a 'b 'c))) (display "\n")
(display "remove 2: ") (display (remove 2 (list 1 2 3 2 4))) (display "\n")
(display "find 4: ") (display (find (lambda (x) (eq? x 4)) (range 0 5))) (display "\n")
(display "forall (>0): ") (display (forall (lambda (x) (> x -1)) (range 0 5))) (display "\n")
(display "exists (=3): ") (display (exists (lambda (x) (eq? x 3)) (range 0 5))) (display "\n")
(display "last: ") (display (last (range 0 5))) (display "\n")
(display "take 3: ") (display (take 3 (range 0 10))) (display "\n")
(display "drop 3: ") (display (drop 3 (range 0 10))) (display "\n")
(display "flatten: ") (display (flatten (list (list 1 (list 2 (list 3 4))) 5))) (display "\n")
(display "zip: ") (display (zip (list 1 2 3) (list 'a 'b 'c))) (display "\n")

(display "\n--- Logical operators ---\n")

(display (not 0) "\n")                                        ; 1
(display (not 1) "\n")                                        ; 0
(display (and 1 1) "\n")                                      ; 1
(display (and 1 0) "\n")                                      ; 0
(display (or 0 0) "\n")                                       ; 0
(display (or 1 0) "\n")                                       ; 1

(display "\n--- Standard Library Stress Test (Large Lists) ---\n")

(define large (range 0 10000)) ; 100k elements

(display "Generated large list of 10000 elements.\n")

(display "Testing map on large list...\n")
(define mapped (map (lambda (x) (+ x 1)) large))

(display "Testing filter on large list (>5000)...\n")
(define filtered (filter (lambda (x) (> x 5000)) large))

(display "Testing fold on large list...\n")
(define total (fold + 0 (take 1000 large)))
(display "Sum of first 1000 elements: ") (display total) (display "\n")

(display "Testing iterate (doubling) 1000 times...\n")
(define itest (iterate (lambda (x) (+ x 2)) 0 1000))

(display "Testing take and drop...\n")
(display (take 5 large)) (display "\n")
(display (drop 9995 large)) (display "\n")

(display "\n=== Demo COmpleted ===\n")

;; eof
