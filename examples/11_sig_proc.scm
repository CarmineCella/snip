(load "stdlib.scm")

(print "\n=== Signal Processing Demo ===\n")

;; Create a simple signal
(define signal (list 0 0.707 1 0.707 0 -0.707 -1 -0.707))

(print "Original signal:\n")
(print signal "\n")

;; Apply FFT
(define spectrum (fft signal))

(print "FFT of signal:\n")
(print spectrum "\n")

;; Convert to polar coordinates
(define polar (car2pol spectrum))

(print "Polar coordinates:\n")
(print polar "\n")

;; Convert back to cartesian
(define cartesian (pol2car polar))

(print "Back to cartesian:\n")
(print cartesian "\n")

;; Inverse FFT
(define reconstructed (ifft cartesian))

(print "Reconstructed signal:\n")
(print reconstructed "\n")

;; --- Fast convolution example ---

;; Create another signal (impulse response)
(define impulse (list 1 0.5 0.25 0))

(print "Impulse response:\n")
(print impulse "\n")

;; Convolve signal with impulse response
(define convolved (conv signal impulse))

(print "Convolved signal:\n")
(print convolved "\n")

(print "\n=== Signal Demo Completed ===\n")
