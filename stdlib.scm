;; Snip standard library
;; 

;; --- higher order ---
(define map
  (lambda (f lst)
    (if (eq? lst '())
        '()
        (cons (f (car lst)) (map f (cdr lst))))))

(define fold
  (lambda (f init lst)
    (if (eq? lst '())
        init
        (fold f (f (car lst) init) (cdr lst)))))

;; --- lists ---
(define filter
  (lambda (pred lst)
    (if (eq? lst '())
        '()
        (if (pred (car lst))
            (cons (car lst) (filter pred (cdr lst)))
            (filter pred (cdr lst))))))

(define element
  (lambda (x lst)
    (if (eq? lst '())
        0
        (if (eq? (car lst) x)
            1
            (element x (cdr lst))))))

(define reverse
  (lambda (lst)
    (fold (lambda (x acc) (cons x acc)) '() lst)))

(define append
  (lambda (lst1 lst2)
    (if (eq? lst1 '())
        lst2
        (cons (car lst1) (append (cdr lst1) lst2)))))

(define length
  (lambda (lst)
    (if (eq? lst '())
        0
        (+ 1 (length (cdr lst))))))

(define assoc
  (lambda (key lst)
    (if (eq? lst '())
        0
        (if (eq? (car (car lst)) key)
            (car lst)
            (assoc key (cdr lst))))))

(define memq
  (lambda (x lst)
    (if (eq? lst '())
        0
        (if (eq? (car lst) x)
            lst
            (memq x (cdr lst))))))

(define remove
  (lambda (x lst)
    (if (eq? lst '())
        '()
        (if (eq? (car lst) x)
            (remove x (cdr lst))
            (cons (car lst) (remove x (cdr lst)))))))

(define find
  (lambda (pred lst)
    (if (eq? lst '())
        0
        (if (pred (car lst))
            (car lst)
            (find pred (cdr lst))))))

(define forall
  (lambda (pred lst)
    (if (eq? lst '())
        1
        (if (pred (car lst))
            (forall pred (cdr lst))
            0))))

(define exists
  (lambda (pred lst)
    (if (eq? lst '())
        0
        (if (pred (car lst))
            1
            (exists pred (cdr lst))))))

(define last
  (lambda (lst)
    (if (eq? (cdr lst) '())
        (car lst)
        (last (cdr lst)))))

(define take
  (lambda (n lst)
    (if (eq? n 0)
        '()
        (cons (car lst) (take (- n 1) (cdr lst))))))

(define drop
  (lambda (n lst)
    (if (eq? n 0)
        lst
        (drop (- n 1) (cdr lst)))))

(define flatten
  (lambda (lst)
    (if (eq? lst '())
        '()
        (if (eq? (car lst) '())
            (flatten (cdr lst))
            (if (eq? (car (car lst)) '())
                (cons (car lst) (flatten (cdr lst)))
                (append (flatten (car lst)) (flatten (cdr lst))))))))

(define zip
  (lambda (lst1 lst2)
    (if (or (eq? lst1 '()) (eq? lst2 '()))
        '()
        (cons (list (cons (car lst1) (car lst2))) (zip (cdr lst1) (cdr lst2))))))

;; --- logical operators ---
(define not
  (lambda (x)
    (if (eq? x 0)
        1
        0)))

(define and
  (lambda (x y)
    (if (eq? x 0)
        0
        (if (eq? y 0)
            0
            1))))

(define or
  (lambda (x y)
    (if (eq? x 0)
        (if (eq? y 0)
            0
            1)
        1)))

;; eof


