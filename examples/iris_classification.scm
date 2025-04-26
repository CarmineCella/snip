;; --- Split data: first 120 for training, last 30 for testing ---

(load "stdlib.scm")
(load "iris.scm")

(define train_x (take 120 iris_x))
(define train_y (take 120 iris_y))
(define test_x (drop 120 iris_x))
(define test_y (drop 120 iris_y))

(print "\n=== kNN Iris Classification Experiment ===\n")

(define correct 0)
(define total (length test_x))

;; classify each test sample
(begin
  (define i 0)
  (while (< i total)
    (begin
      (define predicted (knn train_x train_y (car (drop i test_x)) 3))
      (define true_label (car (drop i test_y)))
      (if (eq? predicted true_label)
          (begin (set! correct (+ correct 1)) 0)
          0)
      (set! i (+ i 1))
    )
  )
)

;; print accuracy
(print "\nCorrect classifications: " correct " out of " total "\n")
(print "Accuracy: " (/ (* correct 100) total) "%\n")

(print "\n=== Experiment completed ===\n")
