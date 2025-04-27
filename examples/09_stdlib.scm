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

;; --- Iterate ---
(print "\n-- Iterate function --\n")
(print (iterate (lambda (x) (+ x 1)) 1 5) "\n")

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


;; Standard Library Demo

(print "--- Standard Library Basic Demo ---\n")

(print "range: ") (print (range 0 10)) (print "\n")
(print "map (x+1): ") (print (map (lambda (x) (+ x 1)) (range 0 5))) (print "\n")
(print "filter (>2): ") (print (filter (lambda (x) (> x 2)) (range 0 5))) (print "\n")
(print "fold (+): ") (print (fold + 0 (range 1 6))) (print "\n")
(print "reverse: ") (print (reverse (range 0 5))) (print "\n")
(print "iterate (double): ") (print (iterate (lambda (x) (* 2 x)) 1 5)) (print "\n")
(print "append: ") (print (append (range 0 3) (range 3 6))) (print "\n")
(print "element 3: ") (print (element 3 (range 0 5))) (print "\n")
(print "length: ") (print (length (range 0 5))) (print "\n")
(print "assoc: ") (print (assoc 'b (list (list 'a 1) (list 'b 2) (list 'c 3)))) (print "\n")
(print "memq 'b: ") (print (memq 'b (list 'a 'b 'c))) (print "\n")
(print "remove 2: ") (print (remove 2 (list 1 2 3 2 4))) (print "\n")
(print "find 4: ") (print (find (lambda (x) (eq? x 4)) (range 0 5))) (print "\n")
(print "forall (>0): ") (print (forall (lambda (x) (> x -1)) (range 0 5))) (print "\n")
(print "exists (=3): ") (print (exists (lambda (x) (eq? x 3)) (range 0 5))) (print "\n")
(print "last: ") (print (last (range 0 5))) (print "\n")
(print "take 3: ") (print (take 3 (range 0 10))) (print "\n")
(print "drop 3: ") (print (drop 3 (range 0 10))) (print "\n")
(print "flatten: ") (print (flatten (list (list 1 (list 2 (list 3 4))) 5))) (print "\n")
(print "zip: ") (print (zip (list 1 2 3) (list 'a 'b 'c))) (print "\n")

;; --- Standard Library Demo ---

(print " -------------- Testing basic list functions\n")

(print (map (lambda (x) (+ x 1)) (list 1 2 3)) "\n")      ; (2 3 4)
(print (filter (lambda (x) (> x 2)) (list 1 2 3 4)) "\n") ; (3 4)
(print (fold + 0 (list 1 2 3 4)) "\n")                    ; 10
(print (element 3 (list 1 2 3 4)) "\n")                   ; 1
(print (reverse (list 1 2 3)) "\n")                       ; (3 2 1)
(print (append (list 1 2) (list 3 4)) "\n")                ; (1 2 3 4)
(print (length (list 5 6 7)) "\n")                         ; 3
(print (assoc 'a (list (list 'a 1) (list 'b 2))) "\n")      ; (a 1)
(print (memq 'b (list 'a 'b 'c)) "\n")                     ; (b c)
(print (remove 2 (list 1 2 3 2 4)) "\n")                   ; (1 3 4)
(print (find (lambda (x) (eq? x 3)) (list 1 2 3 4)) "\n")  ; 3
(print (forall (lambda (x) (< x 5)) (list 1 2 3 4)) "\n")  ; 1
(print (exists (lambda (x) (eq? x 2)) (list 1 3 4)) "\n")  ; 0
(print (last (list 1 2 3 4)) "\n")                         ; 4
(print (take 2 (list 1 2 3 4)) "\n")                       ; (1 2)
(print (drop 2 (list 1 2 3 4)) "\n")                       ; (3 4)
(print (flatten (list 1 (list 2 (list 3 4)) 5)) "\n")       ; (1 2 3 4 5)
(print (zip (list 1 2 3) (list 'a 'b 'c)) "\n")             ; ((1 a) (2 b) (3 c))

(print "Logical operators:\n")

(print (not 0) "\n")                                        ; 1
(print (not 1) "\n")                                        ; 0
(print (and 1 1) "\n")                                      ; 1
(print (and 1 0) "\n")                                      ; 0
(print (or 0 0) "\n")                                       ; 0
(print (or 1 0) "\n")                                       ; 1

(print "done.\n")

;; --- Stress Test ---

(print "\n--- Standard Library Stress Test (Large Lists) ---\n")

(define large (range 0 100000)) ; 100k elements

(print "Generated large list of 100000 elements.\n")

(print "Testing map on large list...\n")
(define mapped (map (lambda (x) (+ x 1)) large))

(print "Testing filter on large list (>50000)...\n")
(define filtered (filter (lambda (x) (> x 50000)) large))

(print "Testing fold on large list...\n")
(define total (fold + 0 (take 1000 large)))
(print "Sum of first 1000 elements: ") (print total) (print "\n")

(print "Testing iterate (doubling) 1000 times...\n")
(define itest (iterate (lambda (x) (+ x 2)) 0 1000))

(print "Testing take and drop...\n")
(print (take 5 large)) (print "\n")
(print (drop 99995 large)) (print "\n")

(print "All stress tests passed without crash!\n")

;; eof
