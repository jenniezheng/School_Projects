(define  (listdiff? obj)
    (define  (listdiffhelper a b)
        (cond   [(eq? a b) #t]
                [(not (pair? a)) #f]
                [#t (listdiffhelper (cdr a) b)]))
    ;checks that a is [stuff] + b
    (cond   [(empty? obj) #f]
            [(not (pair? obj)) #f]
            [(empty? (cdr obj)) #t]
            [#t (listdiffhelper (car obj) (cdr obj))] ))
;Return #t if obj is a listdiff, #f otherwise.

(define (null-ld? obj)
    (if (and (listdiff? obj)
        (eq? (car obj) (cdr obj)) )
        #t
        #f ))
;Return #t if obj is an empty listdiff, #f otherwise.

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

(define (cdr-ld listdiff)
    (if (or (not (listdiff? listdiff))
            (null-ld? listdiff))
        (error "Error")
        (cons (cdr (car listdiff)) (cdr listdiff))))
;Return a listdiff containing all but the first element of listdiff.
;It is an error if listdiff has no elements.

(define (listdiff obj . arguments)
    (cons (cons obj arguments) '()))
;Return a newly allocated listdiff of its arguments.

(define (length-ld listdiff)
    (define (lengthhelper mylist mycount)
        (let ([oneup (+ mycount 1)])
        (cond   [(empty? mylist) mycount]
                [(not (pair? mylist)) oneup]
                [#t (lengthhelper (cdr mylist) oneup)])))
    (if (listdiff? listdiff)
        (- (lengthhelper (car listdiff) 0) (lengthhelper (cdr listdiff) 0))
        (error "Error") ))
;Return the length of listdiff.

(define (listdiff->list listdiff)
     (define (listdiffhelper a b)
        (cond   [(eq? a b) '()]
                [#t (cons (car a) [listdiffhelper (cdr a) b])]))
                ;recursive but not tail recursive since tail recursion won't work here
        ;add first of a to b, recurse
    (if (not (listdiff? listdiff))
        (error "error")
        (listdiffhelper (car listdiff) (cdr listdiff))))
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
        (cond   [(empty? arguments)  listdiff]
                [(empty? (car arguments))  listdiff]
                ;done with arguments

                [(not (listdiff? (car arguments))) (error "Error")]
                [#t (let ([res (addListdiffs listdiff (car arguments))])
                    (append-ld-helper res (cdr arguments)))]))
    ;use helper function to bypass the annoying packing from . arguments which took me hours to debug
    (append-ld-helper listdiff arguments))

;Return a listdiff consisting of the elements of the first listdiff
;followed by the elements of the other listdiffs.
;The resulting listdiff is always newly allocated,
;except that it shares structure with the last argument.
;(Unlike append, the last argument cannot be an arbitrary object; it must be a listdiff.)

(define (list-tail-ld listdiff k)
    (define (remove-k-elements k ld)
        (if (<= k 0)
            ld
            (remove-k-elements (- k 1) (cdr-ld ld))))
    (cond   [(< k 0) (error "Error")]
            [(not (listdiff? listdiff)) (error "Error")]
            [(> k (length-ld listdiff)) (error "Error")]
            [#t (remove-k-elements k listdiff)]))

;Return listdiff, except with the first k elements omitted.
;If k is zero, return listdiff. It is an error if k exceeds the length of listdiff.

(define (expr-returning listdiff)
    (cond   [(listdiff? listdiff) `(cons ',(listdiff->list listdiff) '())]
            [#t (error "Error")]))
    ;constructs expression using listdiff->list as well as quasiquote

;Return a Scheme expression that, when evaluated, will return a
;copy of listdiff, that is, a listdiff that has the same top-level
;data structure as listdiff. Your implementation can assume that the
;argument listdiff contains only booleans, characters, numbers, and symbols.
