;; Save some data
(define data (list
  (list "name" "age")
  (list "Alice" 30)
  (list "Bob" 25)))

(writecsv "test.csv" data)

;; Now read it back
(define loaded (readcsv "test.csv"))

(print loaded "\n")
