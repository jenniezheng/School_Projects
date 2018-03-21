#|
(load "hw5.scm")
(define ils (append '(a e i o u) 'y))
(define d1 (cons ils (cdr (cdr ils))))
(define d2 (cons ils ils))
(define d3 (cons ils (append '(a e i o u) 'y)))
(define d4 (cons '() ils))
(define d5 0)
(define d6 (listdiff ils d1 37))
(define d7 (append-ld d1 d2 d6))
(define e1 (expr-returning d1))

(listdiff? d1)
(listdiff? d2)
(listdiff? d3)
(listdiff? d4)
(listdiff? d5)
(listdiff? d6)
(listdiff? d7)


(null-ld? d1)
(null-ld? d2)
(null-ld? d3)
(null-ld? d6)

(car-ld d1)
(car-ld d2)
(car-ld d3)
(car-ld d6)

(load "hw5.scm")
(length-ld d1)
(length-ld d2)
(length-ld d3)
(length-ld d6)
(length-ld d7)

(define kv1 (cons d1 'a))
(define kv2 (cons d2 'b))
(define kv3 (cons d3 'c))
(define kv4 (cons d1 'd))
(define d8 (listdiff kv1 kv2 kv3 kv4))
(define d9 (listdiff kv3 kv4))
(eq? d8 (list-tail-ld d8 0 ))
(equal? (listdiff->list (list-tail-ld d8 2 ))
        (listdiff->list d9))
(null-ld? (list-tail-ld d8 4 ))
(list-tail-ld  d8 -1 )
(list-tail-ld  d8 5 )

(eq? (car-ld d6) ils)
(eq? (car-ld (cdr-ld d6)) d1)
(eqv? (car-ld (cdr-ld (cdr-ld d6))) 37)
(equal? (listdiff->list d6)
        (list ils d1 37))
(eq? (list-tail (car d6) 3) (cdr d6))

(listdiff->list (eval e1))
(equal? (listdiff->list (eval e1))
        (listdiff->list d1))
|#
#|
(define ap '(a))
(null-ld? (cons ap ap))
|#
(define (null-ld? obj)
    (if (and (pair? obj)
        (eq? (car obj) (cdr obj)) )
        #t
        #f ))

(define  (listdiff? obj)
    (define  (listdiffhelper a b)
        (cond   [(eq? a b) #t]
                [(not (pair? a)) #f]
                [#t (listdiffhelper (cdr a) b)]))
    ;checks that a is [stuff] + b
    (cond   [(null? obj) #f]
            [(not (pair? obj)) #f]
            [(not (pair? (car obj))) #f]
            [(eq? '() (cdr obj)) #t]
            [#t (listdiffhelper (car obj) (cdr obj))] ))
;Return #t if obj is a listdiff, #f otherwise.


;Return #t if obj is an empty listdiff, #f otherwise.
#|
(load "hw5.scm")
(define li '(a b c))
(define d9 (cons li (cdr (cdr li))))
(cons-ld 'r (cons-ld 'd d9))
|#
(define (cons-ld obj listdiff)
    (if (listdiff? listdiff)
        (cons
            (cons obj (car listdiff))
            (cdr listdiff))
        (error "Error")))

;Return a listdiff whose first element is obj and whose remaining elements are listdiff.
;(Unlike cons, the last argument cannot be an arbitrary object; it must be a listdiff.)
(define (car-ld listdiff)
    (if (or
            (not (listdiff? listdiff))
            (null-ld? listdiff)
        )
        (error "Error")
        (car (car listdiff))))

;Return the first element of listdiff.
;It is an error if listdiff has no elements.
;("It is an error" means the implementation can do anything it likes
;when this happens, and we won't test this case when grading.
#|
(load "hw5.scm")
(define li '(a b c))
(define d9 (cons li (cdr (cdr li))))
(cdr-ld d9)
(cdr-ld (cdr-ld d9))

(define li2 '(a b c))
(define d10 (cons li2 (cdr(cdr (cdr li2)))))
(cdr-ld d10)
(cdr-ld (cdr-ld d10))
|#
(define (cdr-ld listdiff)
    (if (or
            (not (listdiff? listdiff))
            (null-ld? listdiff)
        )
        (error "Error")
        (cons (cdr (car listdiff)) (cdr listdiff))))
#|
(load "hw5.scm")
d2
(cdr-ld d1)
(cdr-ld (cdr-ld d1))
(cdr-ld(cdr-ld (cdr-ld d1)))
|#
;Return a listdiff containing all but the first element of listdiff.
;It is an error if listdiff has no elements.


(define (listdiff obj . arguments)
    (cons (cons obj arguments) '()))
;Return a newly allocated listdiff of its arguments.

(define (length-ld listdiff)
    (define (lengthhelper mylist mycount)
        (let ([oneup (+ mycount 1)])
        (cond   [(eq? '() mylist) mycount]
                [(not (pair? mylist)) oneup]
                [#t (lengthhelper (cdr mylist) oneup)])))
    (if (listdiff? listdiff)
        (- (lengthhelper (car listdiff) 0) (lengthhelper (cdr listdiff) 0))
        (error "Error") ))
;Return the length of listdiff.
#|
(load "hw5.scm")
(length-ld d1)
(length-ld d2)
(length-ld d3)
(length-ld d6)
(length-ld d7)
(lengthhelper '(a b c) 0)
|#
(define (listdiff->list listdiff)
     (define (listdiffhelper a b)
        (cond   [(eq? a b) '()]
                [#t (cons (car a) [listdiffhelper (cdr a) b])]))
                ;recursive but not tail recursive since tail recursion won't work here
        ;add first of a to b, recurse
    (if (not (listdiff? listdiff))
        (error "error")
        (listdiffhelper (car listdiff) (cdr listdiff))))

#|
(load "hw5.scm")
(list->listdiff '(a b c))
(define ld (list->listdiff '(a b c)))
(listdiff? ld)
(car-ld ld)
|#
;Return a listdiff that represents the same elements as list.



(define (append-ld listdiff . arguments)
    (define (addListdiffs a b)
    ;adds a to b
        (if (null-ld? a)
            b
            (cons-ld
                (car-ld a)
                (addListdiffs
                    (cdr-ld a)
                    b))))
        ;add listdiff a to listdiff b
    (define (append-ld-helper listdiff arguments)
        (displayln listdiff)
        (displayln arguments)
        (cond   [(eq? '() arguments)  listdiff]
                [(eq? '() (car arguments))  listdiff]
                ;done with arguments

                [(not (listdiff? (car arguments))) (error "Error")]
                [#t (let ([res (addListdiffs listdiff (car arguments))])
                    (append-ld-helper res (cdr arguments)))]))
    ;bypass the annoying packing from . arguments
    (append-ld-helper listdiff arguments))
#|
(load "hw5.scm")
(define ils '(a b c d e))
(define ils2 '(q r s))
(define ils3 '(c a t s))
(define ils4 '(m o u s e))
(define d1 (cons ils (cdr (cdr (cdr ils)))))
(define d2 (cons ils2 (cdr ils2)))
(define d3 (cons ils3 (cdr(cdr(cdr ils3)))))
(define d4 (cons ils4 (cdr(cdr(cdr ils4)))))
(load "hw5.scm")
(append-ld d1 d2 d3)
(append-ld d1 d2 d3 d4)
|#
;Return a listdiff consisting of the elements of the first listdiff
;followed by the elements of the other listdiffs.
;The resulting listdiff is always newly allocated,
;except that it shares structure with the last argument.
;(Unlike append, the last argument cannot be an arbitrary object; it must be a listdiff.)

(define (list-tail-ld k listdiff)
    (define (remove-k-elements k a)
        (if (<= k 0)
            a
            (remove-k-elements (- k 1) (cdr a))))
    (cond   [(< k 0) (error "Error")]
            [(not (listdiff? listdiff)) (error "Error")]
            [(> k (length-ld listdiff)) (error "Error")]
            [#t (cons (remove-k-elements k (car listdiff)) (cdr listdiff))]))
#|
(load "hw5.scm")
(list-tail-ld 0 d9)
|#
;Return listdiff, except with the first k elements omitted.
;If k is zero, return listdiff. It is an error if k exceeds the length of listdiff.

(define (expr-returning listdiff)
    (cond   [(listdiff? listdiff) `(cons ',(listdiff->list listdiff) '())]
            [#t (error "Error")]))
#|
(load "hw5.scm")
(expr-returning d1)
|#
;Return a Scheme expression that, when evaluated, will return a
;copy of listdiff, that is, a listdiff that has the same top-level
;data structure as listdiff. Your implementation can assume that the
;argument listdiff contains only booleans, characters, numbers, and symbols.
