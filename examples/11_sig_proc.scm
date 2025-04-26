(load "stdlib.scm")

(print "\n=== Longer Signal + Convolution Demo ===\n")

;; --- Create a longer signal: mixed sine and cosine ---

(define pi 3.141592653589793)
(define signal
  (list
    0.0
    (sin (* 2 pi 1/8))
    (sin (* 2 pi 2/8))
    (sin (* 2 pi 3/8))
    (sin (* 2 pi 4/8))
    (sin (* 2 pi 5/8))
    (sin (* 2 pi 6/8))
    (sin (* 2 pi 7/8))))

(print "\nOriginal longer signal:\n")
(print signal "\n")

;; --- FFT of the signal ---

(define signal_fft (fft signal))

(print "\nFFT of longer signal (pairs: real, imag):\n")
(print signal_fft "\n")

;; --- Cartesian to Polar and back ---

(define signal_polar (car2pol signal_fft))
(define signal_cartesian (pol2car signal_polar))

(print "\nReconstructed Cartesian after Polar roundtrip:\n")
(print signal_cartesian "\n")

;; --- IFFT (full round trip) ---

(define reconstructed (ifft signal_cartesian))

(print "\nReconstructed longer signal:\n")
(print reconstructed "\n")

;; --- Calculate reconstruction error ---

(define diff
  (map (lambda (x y) (- x y)) signal reconstructed))

(define error (dot diff diff))

(print "\nSquared reconstruction error:\n")
(print error "\n")

;; --- Create a simple filter kernel (lowpass)

(define kernel
  (list 0.25 0.5 0.25 0.0)) ;; Very simple 4-tap lowpass

(print "\nLowpass filter kernel:\n")
(print kernel "\n")

;; --- Convolve signal with filter ---

(define filtered (conv signal kernel))

(print "\nFiltered signal after convolution:\n")
(print filtered "\n")

(print "\n=== Demo Completed ===\n")
