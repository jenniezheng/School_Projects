CC=gcc
CFLAGS=-g
EXE=lab1a
#remove checkfile before turning in!
CHECKFILE=P1A_check.sh
FILES=README $(EXE).c Makefile $(CHECKFILE)
ID=304806663
DIST=lab1a-304806663.tar.gz

.SILENT:

all: $(EXE)

lab1a: $(EXE).c
	$(CC) -o $(EXE) $(EXE).c $(CFLAGS)

clean:
	mkdir /tmp/tempBackup.$(ID)
	mv $(FILES) /tmp/tempBackup.$(ID)/.
	rm -rf *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

dist: 
	tar -czf $(DIST) $(FILES)

#remove before turning in
check2: dist $(CHECKFILE)
	./$(CHECKFILE) $(ID)

check: lab1a
	./lab1a
