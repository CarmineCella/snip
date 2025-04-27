(load "stdlib.scm")

(display "\n=== Signal Processing Demo ===\n")

;; Create a simple signal
(define signal (list 0 0.707 1 0.707 0 -0.707 -1 -0.707))

(display "Original signal:\n")
(display signal "\n")

;; Apply FFT
(define spectrum (fft signal))

(display "FFT of signal:\n")
(display spectrum "\n")

;; Convert to polar coordinates
(define polar (car2pol spectrum))

(display "Polar coordinates:\n")
(display polar "\n")

;; Convert back to cartesian
(define cartesian (pol2car polar))

(display "Back to cartesian:\n")
(display cartesian "\n")

;; Inverse FFT
(define reconstructed (ifft cartesian))

(display "Reconstructed signal:\n")
(display reconstructed "\n")

;; --- Fast convolution example ---

;; Create another signal (impulse response)
(define impulse (list 1 0.5 0.25 0))

(display "Impulse response:\n")
(display impulse "\n")

;; Convolve signal with impulse response
(define convolved (conv signal impulse))

(display "Convolved signal:\n")
(display convolved "\n")

(display "\n=== Signal Demo Completed ===\n")
