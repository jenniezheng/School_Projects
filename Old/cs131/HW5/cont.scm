#lang racket
(define printing-naive-find
  (lambda (pred x)
    (cond ((null? x)      (null))
          ((pred (car x)) (car x))
          (else (let ((retval (printing-naive-find pred (cdr x))))
                  (display "returning from recursive call at: ")
                  (display (car x))
                  (newline)
                  retval)))))

(define printing-find
  (lambda (pred x)
    (call/cc
      (lambda (cont)
        (letrec ((helper
                   (lambda (pred x)
                     (cond ((null? x)      (cont (null)))
                           ((pred (car x)) (cont (car x)))
                           (else (let ((retval (helper pred (cdr x))))
                                    (display "returning from call at: ")
                                    (display (car x))
                                    (newline)
                                    retval))))))
          (helper pred x))))))