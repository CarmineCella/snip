;; Snip Standard Library
;;

;; --- macros ---

(define let
  (macro (bindings body)
    (cons
      (cons 'lambda (cons (map car bindings) (list body)))
      (map (lambda (binding) (car (cdr binding))) bindings))))

;; --- higher-order functions ---

(define map
  (lambda (f lst)
    (define map-iter
      (lambda (l acc)
        (if (eq? l '())
            (reverse acc)
            (map-iter (cdr l) (cons (f (car l)) acc)))))
    (map-iter lst '())))

(define fold
  (lambda (f init lst)
    (define fold-iter
      (lambda (l acc)
        (if (eq? l '())
            acc
            (fold-iter (cdr l) (f (car l) acc)))))
    (fold-iter lst init)))

(define range
  (lambda (start end)
    (define range-iter
      (lambda (s acc)
        (if (>= s end)
            (reverse acc)
            (range-iter (+ s 1) (cons s acc)))))
    (range-iter start '())))

(define iterate
  (lambda (f x n)
    (define iterate-iter
      (lambda (count acc)
        (if (eq? count 0)
            (reverse acc)
            (iterate-iter (- count 1) (cons (f (car acc)) acc)))))
    (iterate-iter n (list x))))

;; --- list manipulation ---

(define filter
  (lambda (pred lst)
    (define filter-iter
      (lambda (l acc)
        (if (eq? l '())
            (reverse acc)
            (filter-iter (cdr l)
                         (if (pred (car l))
                             (cons (car l) acc)
                             acc)))))
    (filter-iter lst '())))

(define element
  (lambda (x lst)
    (define element-iter
      (lambda (l)
        (if (eq? l '())
            0
            (if (eq? (car l) x)
                1
                (element-iter (cdr l))))))
    (element-iter lst)))

(define reverse
  (lambda (lst)
    (define reverse-iter
      (lambda (l acc)
        (if (eq? l '())
            acc
            (reverse-iter (cdr l) (cons (car l) acc)))))
    (reverse-iter lst '())))

(define append
  (lambda (lst1 lst2)
    (fold (lambda (x acc) (cons x acc))
          lst2
          (reverse lst1))))

(define length
  (lambda (lst)
    (define length-iter
      (lambda (l acc)
        (if (eq? l '())
            acc
            (length-iter (cdr l) (+ acc 1)))))
    (length-iter lst 0)))

(define assoc
  (lambda (key lst)
    (define assoc-iter
      (lambda (l)
        (if (eq? l '())
            0
            (if (eq? (car (car l)) key)
                (car l)
                (assoc-iter (cdr l))))))
    (assoc-iter lst)))

(define memq
  (lambda (x lst)
    (define memq-iter
      (lambda (l)
        (if (eq? l '())
            0
            (if (eq? (car l) x)
                l
                (memq-iter (cdr l))))))
    (memq-iter lst)))

(define remove
  (lambda (x lst)
    (define remove-iter
      (lambda (l acc)
        (if (eq? l '())
            (reverse acc)
            (remove-iter (cdr l)
                         (if (eq? (car l) x)
                             acc
                             (cons (car l) acc))))))
    (remove-iter lst '())))

(define find
  (lambda (pred lst)
    (define find-iter
      (lambda (l)
        (if (eq? l '())
            0
            (if (pred (car l))
                (car l)
                (find-iter (cdr l))))))
    (find-iter lst)))

(define forall
  (lambda (pred lst)
    (define forall-iter
      (lambda (l)
        (if (eq? l '())
            1
            (if (pred (car l))
                (forall-iter (cdr l))
                0))))
    (forall-iter lst)))

(define exists
  (lambda (pred lst)
    (define exists-iter
      (lambda (l)
        (if (eq? l '())
            0
            (if (pred (car l))
                1
                (exists-iter (cdr l))))))
    (exists-iter lst)))

(define last
  (lambda (lst)
    (define last-iter
      (lambda (l)
        (if (eq? (cdr l) '())
            (car l)
            (last-iter (cdr l)))))
    (last-iter lst)))

(define take
  (lambda (n lst)
    (define take-iter
      (lambda (count l acc)
        (if (or (eq? count 0) (eq? l '()))
            (reverse acc)
            (take-iter (- count 1) (cdr l) (cons (car l) acc)))))
    (take-iter n lst '())))

(define drop
  (lambda (n lst)
    (define drop-iter
      (lambda (count l)
        (if (or (eq? count 0) (eq? l '()))
            l
            (drop-iter (- count 1) (cdr l)))))
    (drop-iter n lst)))

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
        (cons (list (car lst1) (car lst2)) (zip (cdr lst1) (cdr lst2)))))
)


; ;; --- logical operators ---

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
