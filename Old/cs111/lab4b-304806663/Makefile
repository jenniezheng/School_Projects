CC=gcc
CFLAGS=-lmraa -lm -Wall
SOURCE_FILES=lab4b.c
DIST_FILES=$(SOURCE_FILES) Makefile README
ID=304806663
DIST=lab4b-$(ID).tar.gz

default: build

build: lab4b.c
	$(CC) -o lab4b lab4b.c $(CFLAGS) 

check: build
	echo OFF | ./lab4b --bogus > /dev/null 2>&1;  \
	if [ $$? -ne 1 ]; then echo "Error: Wrong return code upon incorrect argument" >> errors.log; fi;
	echo OFF | ./lab4b --log="tester.txt" --scale=F --period=3 > /dev/null 2>&1;  \
	if [ $$? -ne 0 ]; then echo "Error: Did not accept all parameters" >> errors.log; fi;
	echo -e "START\nSTOP\nPERIOD=3\nSCALE=C\nOFF\n" | ./lab4b --log="tester.txt" --scale=F --period=5 > /dev/null 2>&1;  \
	if [ $$? -ne 0 ]; then echo "Error: Did not accept all commands" >> errors.log; fi;
	if [ ! -s "tester.txt" ]; then echo "Error: Did not create a log file" >> errors.log; fi; \
	if [ -s "tester.txt" ]; then \
		for command in START STOP OFF SCALE PERIOD; do \
			grep $$command tester.txt > /dev/null; \
			if [ $$? -ne 0 ]; then echo "Error: Did not log $$command command" >> errors.log; fi; \
	done; fi; 
	
	if [ -f "tester.txt" ]; then rm tester.txt; fi; 
	if [ -f "errors.log" ]; then cat errors.log; rm -f errors.log;  \
	else echo "All tests passed successfully."; fi; 
	
clean:
	mkdir -p /tmp/tempBackup.$(ID)
	-mv $(DIST_FILES) /tmp/tempBackup.$(ID)/.
	-rm *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

dist: 
	tar -czf $(DIST) $(DIST_FILES)

mycheck:	#only for my personal use 
	./get lab4b.c #scp the file from desktop to edison, bypassing password
	$(CC) $(CFLAGS) -o test lab4b.c
	./test --log=tester.txt


compcheck: #only for my personal use, checks on my computer
	$(CC) -o lab4bComp lab4bComp.c -lm -Wall