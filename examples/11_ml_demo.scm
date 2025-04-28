;; Load standard library
(load "stdlib.scm")

(display "\n=== Minimal Science Pack Demo ===\n")

;; --- mean ---
(display "\n--- Mean of a list ---\n")
(display (mean (list 1 2 3 4 5)) "\n") ; 3

;; --- variance ---
(display "\n--- Variance of a list ---\n")
(display (variance (list 1 2 3 4 5)) "\n") ; 2

;; --- stddev ---
(display "\n--- Standard Deviation of a list ---\n")
(display (stddev (list 1 2 3 4 5)) "\n") ; ~1.4142

;; --- distance ---
(display "\n--- Euclidean distance between two vectors ---\n")
(display (distance (list 1 2 3) (list 4 5 6)) "\n") ; ~5.196

;; --- 1D Linear Regression Example ---

(display "\n--- 1D Linear Regression ---\n")

(define x1 (list 1 2 3 4))
(define y1 (list 2 4 6 8))

(define model1 (linreg x1 y1))

(display "Trained 1D model:\n")
(display model1 "\n") ;; should be close to (intercept 0, slope 2)

;; Predict new point
(define y1_pred (predlin model1 5))

(display "Prediction for x = 5:\n")
(display y1_pred "\n") ;; should be close to 10

;; --- 2D Linear Regression Example ---

(display "\n--- 2D Linear Regression ---\n")

;; Notice x1 and x2 are now independent!
(define x2 (list
    (list 1 5)
    (list 2 7)
    (list 3 6)
    (list 4 8)))

(define y2 (list 15 25 20 30)) ;; Some random targets

(define model2 (linreg x2 y2))

(display "Trained 2D model:\n")
(display model2 "\n")

;; Predict new point
(define y2_pred (predlin model2 (list 5 9)))

(display "Prediction for (5,9):\n")
(display y2_pred "\n")

;; --- kmeans ---
(display "\n--- KMeans clustering (k=2) ---\n")
(define points (list 1 2 10 12))
(display (kmeans points 2) "\n") ; cluster centers near 1.5 and 11

;; --- knn ---
(display "\n--- kNN classification ---\n")
(define train_x (list (list 1) (list 2) (list 10) (list 12)))
(define train_y (list 0 0 1 1))
(define query (list 11))
(display (knn train_x train_y query 2) "\n") ; Should predict class 1


(display "\n=== Demo Completed ===\n")
