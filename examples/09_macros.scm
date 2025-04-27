;; Macros

(display "\n--- lambda ---\n")
(define myif
  (lambda (test then else)
    (if test then else)))

(myif (eq? 0 1)
      (display "then\n")
      (display "else\n"))

(display "\n--- macro ---\n")
(define myif
  (macro (test then else)
    (list 'if test then else)))

(myif (eq? 0 1)
      (display "then\n")
      (display "else\n"))

