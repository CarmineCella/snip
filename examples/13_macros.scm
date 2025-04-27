(print "--- lambda ---\n")
(define myif
  (lambda (test then else)
    (if test then else)))

(myif (eq? 0 1)
      (print "then\n")
      (print "else\n"))

(print "--- macro ---\n")
(define myif
  (macro (test then else)
    (list 'if test then else)))

(myif (eq? 0 1)
      (print "then\n")
      (print "else\n"))

