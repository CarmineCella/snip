;; Load standard library
(load "stdlib.scm")

(print "\n=== Minimal Science Pack Demo ===\n")

;; --- mean ---
(print "\n-- Mean of a list --\n")
(print (mean (list 1 2 3 4 5)) "\n") ; 3

;; --- variance ---
(print "\n-- Variance of a list --\n")
(print (variance (list 1 2 3 4 5)) "\n") ; 2.5

;; --- stddev ---
(print "\n-- Standard Deviation of a list --\n")
(print (stddev (list 1 2 3 4 5)) "\n") ; ~1.58

;; --- distance ---
(print "\n-- Euclidean distance between two vectors --\n")
(print (distance (list 1 2 3) (list 4 5 6)) "\n") ; ~5.196

;; --- 1D Linear Regression Example ---

(print "\n1D Linear Regression:\n")

(define x1 (list 1 2 3 4))
(define y1 (list 2 4 6 8))

(define model1 (linear-regression x1 y1))

(print "Trained 1D model:\n")
(print model1 "\n") ;; should be close to (intercept 0, slope 2)

;; Predict new point
(define y1_pred (predict-linear model1 5))

(print "Prediction for x = 5:\n")
(print y1_pred "\n") ;; should be close to 10

;; --- 2D Linear Regression Example ---

(print "\n2D Linear Regression:\n")

;; Notice x1 and x2 are now independent!
(define x2 (list
    (list 1 5)
    (list 2 7)
    (list 3 6)
    (list 4 8)))

(define y2 (list 15 25 20 30)) ;; Some random targets

(define model2 (linear-regression x2 y2))

(print "Trained 2D model:\n")
(print model2 "\n")

;; Predict new point
(define y2_pred (predict-linear model2 (list 5 9)))

(print "Prediction for (5,9):\n")
(print y2_pred "\n")


(print "\n=== ML Demo Completed ===\n")

;; --- kmeans ---
(print "\n-- KMeans clustering (k=2) --\n")
(define points (list 1 2 10 12))
(print (kmeans points 2) "\n") ; cluster centers near 1.5 and 11

;; --- knn ---
(print "\n-- kNN classification --\n")
(define train_x (list (list 1) (list 2) (list 10) (list 12)))
(define train_y (list 0 0 1 1))
(define query (list 11))
(print (knn train_x train_y query 2) "\n") ; Should predict class 1


(print "\n=== Demo Completed ===\n")
