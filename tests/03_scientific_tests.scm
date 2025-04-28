;;  Scientific/ML/Signal Processing Tests

(load "stdlib.scm")

;; --- Statistics ---

(test (mean (list 1 2 3 4 5)) 3)
(test (variance (list 1 2 3 4 5)) 2)
(test (stddev (list 1 2 3 4 5)) 1.4142135623731)
(test (distance (list 0 0) (list 3 4)) 5)

;; --- Machine Learning ---

(test (kmeans (list 1 1 2 2 9 9) 2) (9 1.5))
(test (linreg (list 1 2 3) (list 2 4 6)) (2 0))
(test (linreg-predict (linreg (list 1 2 3) (list 2 4 6)) (list 4))  8)
(test (knn (list (list 1 1) (list 5 5)) (list 0 1) (list 2 2) 2) 0)

; ;; --- Signal Processing ---

(test (length (fft (list 1 0 0 0))) 4)
(test (ifft (fft (list 1 0 0 0))) (1 0 0 0))

(test (dot (list 1 2 3) (list 4 5 6)) 32)

(test (ifft (pol2car (car2pol (fft (list 1 2 3 4 5 6 7 8))))) (1 2 3 4 5 6 7 8))

; ;; --- File I/O ---

;; write a small WAV
(begin
  (writewav "test.wav" (list (list 0 0 0 0)) 16 44100)
  (define x (readwav "test.wav"))
  (test (length (car x)) 4))

;; write a small CSV
(begin
  (writecsv "test.csv" (list (list 1 2 3) (list 4 5 6)))
  (define y (readcsv "test.csv"))
  (test (length y) 2))

(display "\n--- Tests completed ----\n")
