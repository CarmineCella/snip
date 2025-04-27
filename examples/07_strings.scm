;; String operations

(display (string 'length "hello world") "\n")
(display (string 'find "hello world" "world") "\n")
(display (string 'range "hello world" 0 5) "\n")
(display (string 'replace "foo bar foo" "foo" "baz") "\n") 
(display (string 'split "a,b,c,d" ",") "\n")
(display (string 'regex "abc123" "[0-9]+") "\n")

;; eof
