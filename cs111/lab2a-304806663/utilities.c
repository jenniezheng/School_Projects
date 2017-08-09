#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "utilities.h"
#include "SortedList.h"

//store starting time as global variable
struct timespec start_time;
extern int DEBUG_FLAG, YIELD_FLAG, NUM_THREADS, NUM_ITERATIONS;
extern char* SYNC_TYPE;
extern const int PROGRAM_TYPE;
extern int SPIN_LOCK;
extern pthread_mutex_t MUTEX;

void lock(){
	if(strcmp(SYNC_TYPE,"m")==0) pthread_mutex_lock(&MUTEX);
	else if(strcmp(SYNC_TYPE,"s")==0) while(__sync_lock_test_and_set(&SPIN_LOCK, 1));
}

void unlock(){
	if (strcmp(SYNC_TYPE,"s")==0) __sync_lock_release(&SPIN_LOCK);
	else if(strcmp(SYNC_TYPE,"m")==0) pthread_mutex_unlock(&MUTEX);
}

void close_and_exit_program(int return_code){
	if(DEBUG_FLAG)
		printf("Exiting with return code %d\n",return_code);
    exit(return_code);
}

void start_timer(){
	if(clock_gettime(CLOCK_MONOTONIC, &start_time) == -1) { perror("Error: Trouble getting start time"); close_and_exit_program(1); }
}

long long end_timer(){
	/*
	Note timespec struct contains the following:

	struct timespec {
	    time_t   tv_sec;        //seconds 
	    long     tv_nsec;       //nanoseconds

	This function returns the number of nanoseconds (real time) that passed between start_timer and end_timer
	*/
	struct timespec end_time;
	if(clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) { perror("Error: Trouble getting end time"); close_and_exit_program(1); }
	return (long long)1E9 * (end_time.tv_sec - start_time.tv_sec) + ( end_time.tv_nsec - start_time.tv_nsec );
}


//Called when inappropriate argument recieved
void print_usage(){
	char *name, *yield_explanation, *sync_explanation, *program_description;
	if(PROGRAM_TYPE==ADD_TYPE) {
		name="lab2_add";
		sync_explanation="a mutex (m), a spin lock (s), or the compare-and-swap operation (c)";
		yield_explanation="specify yielding with a combition of insert (i), delete (d), and lookup (l)";
		program_description="Creates numerous threads that all add and then subtract one from a global counter.";
	}
	else {
		name="lab2_list";
		sync_explanation="a mutex (m) or a spin lock (s)";
		yield_explanation="ask thread to yield at critical section";
		program_description="Creates numerous threads that all insert and then delete a random element from a global sorted linked list.";
	}
	fprintf(stderr, "Usage: %s [OPTION]... \
\n\r%s\
\n\rOptional arguments:\
\n\r--threads=NUM_THREADS\t\tspecify the number of threads. Default=1.\
\n\r--iterations=NUM_ITERATIONS\tspecify the number of iterations. Default=1.\
\n\r--yield\t\t\t\t%s\
\n\r--sync=SYNC_TYPE\t\tspecify handling of critical section using %s\
\n\r--debug\t\t\t\tprint debug messages\n", name, program_description, yield_explanation, sync_explanation);
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

void print_arguments_description(){
	char name[20];
	memset(name,0,sizeof(name));
	if(PROGRAM_TYPE==ADD_TYPE) strcat(name, "add");
	else strcat(name, "list");
	if(PROGRAM_TYPE==ADD_TYPE) {
		if(YIELD_FLAG) strcat(name, "-yield");
	}
	else { 
		strcat(name, "-");
		if(!YIELD_FLAG) strcat(name, "none");
		if(YIELD_FLAG & INSERT_YIELD)strcat(name, "i");
		if(YIELD_FLAG & DELETE_YIELD)strcat(name, "d");
		if(YIELD_FLAG & LOOKUP_YIELD)strcat(name, "l");
	}
	strcat(name, "-");
	strcat(name, SYNC_TYPE);
	fprintf(stdout, "%s,%d,%d,", name, NUM_THREADS, NUM_ITERATIONS); 
}