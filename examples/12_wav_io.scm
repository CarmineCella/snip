(load "stdlib.scm")

(print "\n=== WAV Demo ===\n")

;; --- Create two simple signals: a sine wave and a cosine wave

(define pi 3.141592653589793)
(define sr 44100) ;; Sample rate
(define dur 1) 
(define n (floor (* sr dur)))

(print "Generating " n " samples\n")

;; Generate a sine wave (channel 1)
(define points (iterate (lambda (x) (+ x 0.01)) 0 n))

;; Sine wave
(define sine (map (lambda (x) (sin (* 2 3.1415 x))) points))

;; Cosine wave
(define cosine (map (lambda (x) (cos (* 2 3.1415 x))) points))

;; Pack into multichannel structure
(define stereo (list sine cosine))

;; --- Write to 16-bit WAV file

(print "Saving 16-bit stereo WAV...\n")
(writewav "out16.wav" stereo 16 sr)

;; --- Write to 32-bit WAV file

(print "Saving 32-bit stereo WAV...\n")
(writewav "out32.wav" stereo 32 sr)

;; --- Read back the 16-bit WAV

(print "Reading back 16-bit WAV...\n")
(define loaded16 (readwav "out16.wav"))

;; --- Read back the 32-bit WAV

(print "Reading back 32-bit WAV...\n")
(define loaded32 (readwav "out32.wav"))

;; --- Print summary

(print "\nOriginal first 5 samples (channel 1):\n")
(print (take 5 sine) "\n")

(print "\nLoaded 16-bit first 5 samples (channel 1):\n")
(print (take 5 (car loaded16)) "\n")

(print "\nLoaded 32-bit first 5 samples (channel 1):\n")
(print (take 5 (car loaded32)) "\n")

(print "\n=== WAV Demo Completed ===\n")
