CC=gcc
CFLAGS= -lm -Wall -g 
EDISONFLAG=-lmraa
SSLFLAGS=-lssl -lcrypto 
SOURCE_FILES=lab4c.c connectTCP.c connectTLS.c connect.h
DIST_FILES=$(SOURCE_FILES) Makefile README
ID=304806663
DIST=lab4c-$(ID).tar.gz

default: build

build: $(SOURCE_FILES)
	#builds differently depending on whether on edison
	uname -r | grep "edison"; \
	if [ $$? -ne 1 ]; \
	then $(CC) -o lab4c_tcp connectTCP.c lab4c.c $(CFLAGS) $(EDISONFLAG); \
		 $(CC) -o lab4c_tls connectTLS.c lab4c.c $(CFLAGS) $(EDISONFLAG) $(SSLFLAGS); \
	else $(CC) -o lab4c_tcp connectTCP.c lab4c.c $(CFLAGS); \
		 $(CC) -o lab4c_tls connectTLS.c lab4c.c $(CFLAGS) $(SSLFLAGS); \
	fi;
	
clean:
	mkdir -p /tmp/tempBackup.$(ID)
	-mv $(DIST_FILES) /tmp/tempBackup.$(ID)/.
	-rm *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

dist: 
	tar -czf $(DIST) $(DIST_FILES)

###########################TESTING UTILITIES##########################
runA: build
	./lab4c_tcp --log="logTCP.txt" --debug 18000

runB: build
	./lab4c_tls --log="logTLS.txt" --debug 19000
