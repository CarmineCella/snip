;; Standard Library Tests

(load "stdlib.scm")

;; --- basic list operations ---

(test (length (list 1 2 3)) 3)
(test (map (lambda (x) (+ x 1)) (list 1 2 3)) (2 3 4))
(test (fold + 0 (list 1 2 3 4)) 10)
(test (range 0 5) (0 1 2 3 4))
(test (filter (lambda (x) (eq? x 2)) (list 1 2 3 2 4)) (2 2))
(test (element 2 (list 1 2 3)) 1)
(test (reverse (list 1 2 3)) (3 2 1))
(test (append (list 1 2) (list 3 4)) (1 2 3 4))
(test (assoc 'a (list (list 'a 1) (list 'b 2))) (a 1))
(test (memq 'b (list 'a 'b 'c)) (b c))
(test (remove 2 (list 1 2 3 2 4)) (1 3 4))
(test (find (lambda (x) (eq? x 3)) (list 1 2 3 4)) 3)
; (test (forall (lambda (x) (>= x 0)) (list 1 2 3)) 1)
(test (exists (lambda (x) (eq? x 2)) (list 1 2 3)) 1)
(test (last (list 1 2 3 4)) 4)
(test (take 3 (list 1 2 3 4 5)) (1 2 3))
(test (drop 2 (list 1 2 3 4)) (3 4))
(test (flatten (list (list 1 2) (list 3 4))) (1 2 3 4))
(test (zip (list 1 2) (list 'a 'b)) ((1 a) (2 b)))

;; --- logical operators ---    

(test (not 0) 1)
(test (not 1) 0)
(test (and 1 1) 1)
(test (and 1 0) 0)
(test (or 1 0) 1)
(test (or 0 0) 0)
(test (or 0 1) 1)

;; --- stress test with large list ---

(test (length (range 0 10000)) 10000)

(display "\n--- Tests completed ---\n")

;; eof
