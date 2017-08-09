//NAME: Yunjing Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utilities.h"

const int PROGRAM_TYPE=ADD_TYPE;
// can adjust using process arguments
int DEBUG_FLAG, YIELD_FLAG, NUM_THREADS=1, NUM_ITERATIONS=1;
long long COUNTER;
char* SYNC_TYPE="none";

int SPIN_LOCK;
pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;

void add(long long *pointer, long long value) {
	lock();
    long long sum = *pointer + value;
     if (YIELD_FLAG) sched_yield();
     *pointer = sum;
    unlock();
}

void add_c(long long *pointer, long long value) {
/*
	atomic instruction __sync_val_compare_and_swap

	long long __sync_val_compare_and_swap(long long *ptr, long long prev, long long new){
		long long old=*ptr;
		if(*ptr==prev)*ptr=new;
		return old;
	}
*/
	long long old,new;
	do {
		old=*pointer;
		new=old+value;
		if (YIELD_FLAG) sched_yield();
	}
	while(__sync_val_compare_and_swap(pointer,old,new)!=old);
}

void* thread_task(void* func){
	void (*add_func)(long long*, long long)=func;
	int index_iteration;
	for(index_iteration=0; index_iteration< NUM_ITERATIONS; index_iteration++){
		add_func(&COUNTER,1);
	}
	for(index_iteration=0; index_iteration< NUM_ITERATIONS; index_iteration++){
		add_func(&COUNTER,-1);
	}
	return NULL;
}

long long thread_spawner(){
	void (*func)(long long*, long long);
	if(strcmp(SYNC_TYPE,"c")==0) func=add_c;
	else func=add;
	pthread_t* my_threads=(pthread_t*) calloc(NUM_THREADS, sizeof(pthread_t));
	start_timer();
	int index_thread;
	for(index_thread=0; index_thread< NUM_THREADS; index_thread++){
		if(pthread_create(my_threads+index_thread, NULL, thread_task, (void*)func)) {
			perror("Error: failed to create pthread");
			close_and_exit_program(1);
		}	
	}
	for(index_thread=0; index_thread< NUM_THREADS; index_thread++){
		/* wait for threads to finish*/
		if(pthread_join(my_threads[index_thread], NULL)) {
			perror("Error: failed to join pthread");
			close_and_exit_program(1);
		}
	}
	long long elapsed_time=end_timer();
	free(my_threads);
	return elapsed_time;
}


int main (int argc, char **argv)
{
	process_args(argc, argv);
	long long elapsed_time=thread_spawner();
	long long operations=NUM_THREADS*NUM_ITERATIONS*2;
	print_arguments_description();
	fprintf(stdout, "%lld, %lld, %lld,%lld\n", operations, elapsed_time, elapsed_time/operations, COUNTER); 
	close_and_exit_program(0);
	return 0;
}