CC=gcc
CFLAGS=-g -lpthread -Wall
SOURCE_FILES=lab2_list.c utilities.h utilities.c SortedList.c SortedList.h
PROF_FILE=profile.out
GRAPHING_FILE=lab2_list.gp 
GRAPHS=lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png
DATA_FILES=lab2b_1.csv lab2b_2.csv lab2b_3.csv lab2b_4.csv lab2b_5.csv lab2b_list.csv
CLEAN_KEEP_FILES=Makefile README $(SOURCE_FILES) $(GRAPHING_FILE) $(PROF_FILE)
DIST_FILES=$(CLEAN_KEEP_FILES) $(GRAPHS) $(DATA_FILES) 
ID=304806663
DIST=lab2b-$(ID).tar.gz
LOWER_TRDS=1 2 3 4 5 6 7 8 10 12 14 16
ALL_TRDS=$(LOWER_TRDS) 18 20 22 24
LOWER_ITERS=1 2 4 8 16 32 64 


default: build

build: lab2_list.c utilities.c utilities.h SortedList.c
	$(CC) -o lab2_list lab2_list.c utilities.c SortedList.c $(CFLAGS)

clean:
	mkdir -p /tmp/tempBackup.$(ID)
	-mv $(CLEAN_KEEP_FILES) /tmp/tempBackup.$(ID)/.
	-rm *
	mv /tmp/tempBackup.$(ID)/* . 
	rmdir /tmp/tempBackup.$(ID)

test1: build
	rm -f lab2b_1.csv
	#lab2b_1.png ... throughput vs number of threads for mutex and spin-lock synchronized list operations.
    #Mutex synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads
    #Spin-lock synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads

	for thread_num in $(ALL_TRDS); do \
		for sync_type in s m ; do \
			./lab2_list --threads=$$thread_num --iterations=1000 --sync=$$sync_type >> lab2b_1.csv; \
		done \
	done

test2: build
	rm -f lab2b_2.csv
	#lab2b_2.png ... mean time per mutex wait and mean time per operation for mutex-synchronized list perations.
	#Mutex synchronized list operations, 1,000 iterations, 1,2,4,8,12,16,24 threads

	for thread_num in $(ALL_TRDS); do \
		./lab2_list --threads=$$thread_num --iterations=1000 --sync=m >> lab2b_2.csv; \
	done

test3: build
	rm -f lab2b_3.csv
	#lab2b_3.png ... successful iterations vs threads for each synchronization method.
	#--yield=id, 4 lists, 1,4,8,12,16 threads, and 1, 2, 4, 8, 16 iterations, --sync=none
	#--yield=id, 4 lists, 1,4,8,12,16 threads, and 10, 20, 40, 80 iterations, --sync=s and --sync=m
    
    #modify thread num and iter num later

    #note I intentionally the iter num so that points are visible
	-for thread_num in $(LOWER_TRDS); do \
		for iteration_num in 1 2 4 8 16 32 64; do \
			./lab2_list --yield=id --list=4 --threads=$$thread_num --iterations=$$iteration_num >> lab2b_3.csv; \
		done \
	done

	for thread_num in $(LOWER_TRDS); do \
		for iteration_num in 1 3 6 12 24 48 96; do \
			./lab2_list --yield=id --list=4 --threads=$$thread_num --iterations=$$iteration_num --sync=s >> lab2b_3.csv; \
		done \
	done

	for thread_num in $(LOWER_TRDS); do \
		for iteration_num in 1 5 10 20 40 80; do \
			./lab2_list --yield=id --list=4 --threads=$$thread_num --iterations=$$iteration_num --sync=m >> lab2b_3.csv; \
		done \
	done


test45: build
	rm -f lab2b_4.csv lab2b_5.csv
	for thread_num in $(LOWER_TRDS); do \
		for list_num in 1 4 8 16; do \
			./lab2_list --list=$$list_num --threads=$$thread_num --iterations=1000 --sync=s >> lab2b_5.csv; \
			./lab2_list --list=$$list_num --threads=$$thread_num --iterations=1000 --sync=m >> lab2b_4.csv; \
		done \
	done
    
profile: 
	LD_PRELOAD=~/lib/libprofiler.so.0 $(CC) -o lab2_list lab2_list.c utilities.c SortedList.c $(CFLAGS)
	LD_PRELOAD=~/lib/libprofiler.so.0 CPUPROFILE=temp_profile ./lab2_list --threads=12 --iterations=1000 --sync=s
	~/bin/pprof --list=lock lab2_list temp_profile > profile.out
	rm temp_profile

tests: test1 test2 test3 test45 
	cat lab2b_1.csv lab2b_2.csv lab2b_3.csv lab2b_4.csv lab2b_5.csv > lab2b_list.csv

graphs: 
	/usr/local/cs/bin/gnuplot ./lab2_list.gp

tarer:
	tar -czf $(DIST) $(DIST_FILES)

dist: build tests graphs tarer
