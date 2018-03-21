//NAME: Yunjing Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
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


//this function checks for arguments
void process_args(int argc, char **argv){
	static struct option long_options_list[] =
	{
	  	{"threads", required_argument, 0, 't'}, 
		{"iterations", required_argument, 0, 'i'},
		{"sync", required_argument, 0, 's'},
		{"yield", optional_argument, 0, 'y'},
	    {"debug", no_argument, 0, 'd'},
	    {0, 0, 0, 0}
	};

	int c=0;
	while ((c = getopt_long(argc, argv, "t:i:s:y:d:", long_options_list, NULL)) != -1){
	    //no argument, break
	    if (c == -1) break;
	    switch (c)
	    {
	      case 't': 
	      	NUM_THREADS=atoi(optarg);
	      	if(NUM_THREADS<1) {
	      		fprintf(stderr,"Error: number of threads must be greater than 0.\n");
	      		close_and_exit_program(1);
	      	}
	      	break;
	      case 'i': 
	     	NUM_ITERATIONS=atoi(optarg);
	        if(NUM_ITERATIONS<1) {
	      		fprintf(stderr,"Error: number of iterations must be greater than 0.\n");
	      		close_and_exit_program(1);
	      	}
	      	break;
	       case 's': 
	        if(strcmp(optarg,"none")!=0 && strcmp(optarg,"s")!=0 && strcmp(optarg,"m")!=0 && 
	        	(PROGRAM_TYPE==LIST_TYPE || strcmp(optarg,"c")!=0)){
	       		if(PROGRAM_TYPE==ADD_TYPE) fprintf(stderr,"Error: sync type must be s, c, or m.\n");
	      		else fprintf(stderr,"Error: sync type must be s or m.\n");
	      		close_and_exit_program(1);
	        }
	        SYNC_TYPE=optarg;
	      	break;
	      case 'y':
	      	if(PROGRAM_TYPE==LIST_TYPE) {
	      		if(!optarg){
	      			fprintf(stderr, "Error: yield requires an argument.\n");
	      			close_and_exit_program(1);
	      		}
	      		if((strcmp(optarg,"none")==0)){
	      			YIELD_FLAG=0;
	      		}
	      		else{
	      			int c;
		      		for(c=0; c<strlen(optarg); c++){
		      			switch (optarg[c]){
		      				case 'i':YIELD_FLAG |= INSERT_YIELD;
		      					break;
		      				case 'd':YIELD_FLAG |= DELETE_YIELD;
		      					break;
		      				case 'l':YIELD_FLAG |= LOOKUP_YIELD;
		      					break;
		      				default: fprintf(stderr,"Error: yield argument must be a combition of i, d, and l.\n");
		      					close_and_exit_program(1);
		      			}
	      			}
	      		}
	      	}
	        else{
	        	if(optarg){
					fprintf(stderr, "Error: yield takes no argument.\n");
		      		close_and_exit_program(1);
		      	}
		      	YIELD_FLAG=1;
	        }
	        break;
	      case 'd':
	        DEBUG_FLAG=1;
	        break;
	      case '?':
	      	print_usage();
	      	close_and_exit_program(1);
	      default:
	        // should never get here. 
	      	if(DEBUG_FLAG) printf("Error: Unexpected case.\n");
	        close_and_exit_program(1);
	    }
  	}
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