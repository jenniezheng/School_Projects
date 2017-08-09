CC=gcc
CFLAGS=-g -lpthread -Wall
SOURCE_FILES=lab2_add.c lab2_list.c utilities.h utilities.c SortedList.c SortedList.h
GRAPH_GENERATION_FILES=lab2_add.gp lab2_list.gp
ADD_GRAPHS=lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png
LIST_GRAPHS=lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png
ADD_OUTPUT_FILE=lab2_add.csv
LIST_OUTPUT_FILE=lab2_list.csv
GRAPH_GENERATION_FILES=lab2_list.gp lab2_add.gp
CLEAN_FILES=Makefile README $(SOURCE_FILES) $(GRAPH_GENERATION_FILES)
DIST_FILES=Makefile README $(SOURCE_FILES) $(ADD_GRAPHS) $(LIST_GRAPHS) $(ADD_OUTPUT_FILE) $(LIST_OUTPUT_FILE) $(GRAPH_GENERATION_FILES)
ID=304806663
DIST=lab2a-$(ID).tar.gz
LOWER_TRDS=1 2 3 4 5 6 7 8 10 12
ALL_TRDS=$(LOWER_TRDS) 14 16 18 20 22 24
LOWER_ITERS=1 2 4 8 16 32 64 128 256 512 1024 
UPPER_ITERS=$(LOWER_ITERS) 2048 4096 8192 16384 32768
ALL_ITERS=$(UPPER_ITERS) 65536 131072

default: lab2_add lab2_list

build: lab2_add lab2_list

lab2_add: lab2_add.c utilities.c utilities.h
	$(CC) -o lab2_add lab2_add.c utilities.c $(CFLAGS)

lab2_list: lab2_list.c utilities.c utilities.h SortedList.c
	$(CC) -o lab2_list lab2_list.c utilities.c SortedList.c $(CFLAGS)

clean:
	mkdir -p /tmp/tempBackup.$(ID)
	mv $(CLEAN_FILES) /tmp/tempBackup.$(ID)/.
	rm *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

testadd: lab2_add
	rm -f lab2_add.csv
	#Add-1: Threads and Iterations that run without failure: how many threads/iterations we can run without failure w and w/out yielding
	-for iteration_num in $(ALL_ITERS) ; do \
		for thread_num in $(LOWER_TRDS); do \
			./lab2_add --yield --threads=$$thread_num --iterations=$$iteration_num >> $(ADD_OUTPUT_FILE);\
			./lab2_add --threads=$$thread_num --iterations=$$iteration_num >> $(ADD_OUTPUT_FILE);\
		done \
	done
	#Add-2: The cost of yielding: per operation times vs #threads/iterations w/ and w/o yielding
	-for iteration_num in $(ALL_ITERS) ; do \
		./lab2_add --yield --iterations=$$iteration_num --threads=2 >> $(ADD_OUTPUT_FILE);\
		./lab2_add --yield --iterations=$$iteration_num --threads=8 >> $(ADD_OUTPUT_FILE);\
		./lab2_add --iterations=$$iteration_num --threads=2 >> $(ADD_OUTPUT_FILE);\
		./lab2_add --iterations=$$iteration_num --threads=8 >> $(ADD_OUTPUT_FILE);\
	done

	#Add-3: per operation cost vs number of iterations, 1 thread, no yield
	for iteration_num in $(ALL_ITERS) ; do \
		./lab2_add --iterations=$$iteration_num >> $(ADD_OUTPUT_FILE); \
	done 

	#Add-4: Threads and Iterations that run without failure w/ yield
	-for iteration_num in $(ALL_ITERS)  ; do \
		for thread_num in $(LOWER_TRDS); do \
			for sync_type in s c m; do \
				./lab2_add --yield --threads=$$thread_num --iterations=$$iteration_num --sync=$$sync_type >> $(ADD_OUTPUT_FILE);\
			done \
		done \
	done

	#Add-5: per operation cost vs number of threads, no yield
	for thread_num in $(LOWER_TRDS); do \
		for sync_type in s c m none; do \
			./lab2_add --threads=$$thread_num --iterations=10000 --sync=$$sync_type >> $(ADD_OUTPUT_FILE);\
		done \
	done
	./lab2_add --threads=1 --iterations=10000 --sync=none >> $(ADD_OUTPUT_FILE);\



testlist: lab2_list
	rm -f lab2_list.csv
	#List-1: Cost per Operation vs Iterations for single threaded, nonyield, unprotected
	for iteration_num in $(UPPER_ITERS) ; do \
		./lab2_list --iterations=$$iteration_num >> $(LIST_OUTPUT_FILE);\
	done 

	#List-2: Unprotected Threads and Iterations that run without failure, no protection
	-for iteration_num in $(LOWER_ITERS) ; do \
		for thread_num in $(LOWER_TRDS); do \
			for yield_type in i d il dl none; do \
				./lab2_list --yield=$$yield_type --threads=$$thread_num --iterations=$$iteration_num >> $(LIST_OUTPUT_FILE) 2> /dev/null ;\
			done \
		done \
	done 
	#List-3: Protected Iterations that run without failure
	-for iteration_num in $(LOWER_ITERS); do \
		for yield_type in i d il dl ; do \
			for sync_type in s m none; do \
				./lab2_list --yield=$$yield_type --threads=12 --iterations=$$iteration_num --sync=$$sync_type >> $(LIST_OUTPUT_FILE) 2> /dev/null;\
			done \
		done \
	done 
	#List-4: Scalability of synchronization mechanisms
	for thread_num in $(ALL_TRDS); do \
		for sync_type in s m ; do \
			./lab2_list --threads=$$thread_num --iterations=1000 --sync=$$sync_type >> $(LIST_OUTPUT_FILE);\
		done \
	done

tests: testadd testlist

graphs: 
	./lab2_add.gp 
	./lab2_list.gp 

dist: build tests graphs
	tar -czf $(DIST) $(DIST_FILES)

tarer:
	tar -czf $(DIST) $(DIST_FILES)