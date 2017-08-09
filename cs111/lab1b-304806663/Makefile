CC=gcc
CFLAGS=-g -lmcrypt
EXE1=lab1b-client
EXE2=lab1b-server
HELPER=crypt.h 

FILES=README lab1b-client.c lab1b-server.c crypt.h Makefile my.key
ID=304806663
DIST=lab1b-$(ID).tar.gz
PORT=4000
HOST=localhost
KEY_FILE=my.key
LOG_FILE=log.txt
SERVER_ARGS=--port=$(PORT) --encrypt=$(KEY_FILE) 
CLIENT_ARGS=--port=$(PORT) --host=$(HOST) --log=$(LOG_FILE) --encrypt=$(KEY_FILE) 
#Unused: --debug

default: client server

client: $(EXE1).c
	$(CC) -o $(EXE1) $(EXE1).c $(HELPER) $(CFLAGS)

server: $(EXE2).c
	$(CC) -o $(EXE2) $(EXE2).c $(HELPER) $(CFLAGS)

clean:
	mkdir /tmp/tempBackup.$(ID)
	mv $(FILES) /tmp/tempBackup.$(ID)/.
	rm -r *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

dist: 
	tar -czf $(DIST) $(FILES)

checkclient: client
	./lab1b-client $(CLIENT_ARGS)

checkserver: server
	./lab1b-server $(SERVER_ARGS)

#convenient but not recommended. sometimes & doesn't work.
checkboth: client server
	./lab1b-server $(SERVER_ARGS) & ./lab1b-client $(CLIENT_ARGS)

