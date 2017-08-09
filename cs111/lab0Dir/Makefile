CC=gcc
CFLAGS=-g
FILES=README lab0.c Makefile backtrace.png breakpoint.png

.SILENT:

lab0: lab0.c
	$(CC) -o lab0 lab0.c $(CFLAGS)

check: lab0
	#regular check with input option
	echo "Hello World\nBye World\n" > makeFileTest.txt
	./lab0 --input makeFileTest.txt > /dev/null 2>&1;  \
	if [ $$? -ne 0 ]; then echo "Error: Wrong regular read/write return code" >> errors.log; fi;
	#regular check with input and output options
	./lab0 --input makeFileTest.txt --output makeFileTest2.txt > /dev/null 2>&1;   \
	if [ $$? -ne 0 ]; then echo "Error: Wrong regular read/write return code" >> errors.log; fi;
	chmod 755 makeFileTest2.txt
	cmp --silent makeFileTest.txt makeFileTest2.txt || echo "Error: Written file not identical to original">> errors.log
	#unrecognized argument check
	./lab0 --seggy > /dev/null 2>&1;  \
	if [ $$? -ne  1 ]; then  echo "Error: wrong-argument error not caught">> errors.log; fi;
	#input file error check
	chmod u-r makeFileTest.txt
	./lab0 --input makeFileTest.txt > /dev/null 2>&1;    \
	if [ $$? -ne 2 ]; then  echo "Error: Cannot-open-input-file error not caught">> errors.log; fi;
	#output file error check
	chmod u-w makeFileTest2.txt
	./lab0 --output makeFileTest2.txt > /dev/null 2>&1;   \
	if [ $$? -ne 3 ]; then  echo "Error: Cannot-create-output-file error not caught">> errors.log;  fi;
	#segfault and catch check
	./lab0 --segfault --catch > /dev/null 2>&1;   \
	if [ $$? -ne 4 ]; then echo "Error: segfault not caught">> errors.log; fi;
	rm -f makeFileTest.txt makeFileTest2.txt 
	if [ -f "errors.log" ]; then cat errors.log; rm -f errors.log;  \
	else echo "All tests passed successfully."; fi;

clean:
	mkdir backup 
	mv $(FILES) backup/.
	find . -maxdepth 1 -type f -delete
	mv backup/* . 
	rmdir backup

dist:
	mkdir lab0Dir 
	cp $(FILES) lab0Dir/.
	tar cvzf lab0-304806663.tar.gz lab0Dir
	rm -r lab0Dir
