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

extern int DEBUG_FLAG, YIELD_FLAG, NUM_THREADS, NUM_ITERATIONS, NUM_LISTS;
extern char* SYNC_TYPE;
extern int* SPIN_LOCK;
extern pthread_mutex_t* MUTEX;

//returns wait time
long long lock(int num){
	if(strcmp(SYNC_TYPE,"none")==0)return 0;
	struct timespec start_time;
	start_timer(&start_time);
	if(strcmp(SYNC_TYPE,"m")==0) pthread_mutex_lock(MUTEX+num);
	else if(strcmp(SYNC_TYPE,"s")==0) while(__sync_lock_test_and_set(SPIN_LOCK+num, 1));
	return end_timer(&start_time);
}

void unlock(int num){
	if (strcmp(SYNC_TYPE,"s")==0) __sync_lock_release(SPIN_LOCK+num);
	else if(strcmp(SYNC_TYPE,"m")==0) pthread_mutex_unlock(MUTEX+num);
}

void close_and_exit_program(int return_code){
	if(DEBUG_FLAG)
		printf("Exiting with return code %d\n",return_code);
    exit(return_code);
}

void start_timer(struct timespec* start_time){
	if(clock_gettime(CLOCK_MONOTONIC, start_time) == -1) { perror("Error: Trouble getting start time"); close_and_exit_program(1); }
}

long long end_timer(struct timespec* start_time){
	/*
	Note timespec struct contains the following:

	struct timespec {
	    time_t   tv_sec;        //seconds 
	    long     tv_nsec;       //nanoseconds

	This function returns the number of nanoseconds (real time) that passed between start_timer and end_timer
	*/
	struct timespec end_time;
	if(clock_gettime(CLOCK_MONOTONIC, &end_time) == -1) { perror("Error: Trouble getting end time"); close_and_exit_program(1); }
	return (long long)1E9 * (end_time.tv_sec - start_time->tv_sec) + ( end_time.tv_nsec - start_time->tv_nsec );
}

//Called when inappropriate argument recieved
void print_usage(){
	char *name, *program_description;
	name="lab2_list";
	program_description="Creates numerous threads that all insert and then delete a random element from several global sorted linked lists.";
	fprintf(stderr, "Usage: %s [OPTION]... \
\n\r%s\
\n\rOptional arguments:\
\n\r--list=NUM_LISTS\t\tspecify the number of lists. Default=1.\
\n\r--threads=NUM_THREADS\t\tspecify the number of threads. Default=1.\
\n\r--iterations=NUM_ITERATIONS\tspecify the number of iterations. Default=1.\
\n\r--yield\t\t\t\task thread to yield at critical section\
\n\r--sync=SYNC_TYPE\t\tspecify handling of critical section using a mutex (m) or a spin lock (s)\
\n\r--debug\t\t\t\tprint debug messages\n", name, program_description);
}

//this function checks for arguments
void process_args(int argc, char **argv){
	static struct option long_options_list[] =
	{
		{"lists", required_argument, 0, 'l'}, 
	  	{"threads", required_argument, 0, 't'}, 
		{"iterations", required_argument, 0, 'i'},
		{"sync", required_argument, 0, 's'},
		{"yield", required_argument, 0, 'y'},
	    {"debug", no_argument, 0, 'd'},
	    {0, 0, 0, 0}
	};

	int c=0;
	while ((c = getopt_long(argc, argv, "l:t:i:s:y:d:", long_options_list, NULL)) != -1){
	    //no argument, break
	    if (c == -1) break;
	    switch (c)
	    {
    	case 'l': 
      	NUM_LISTS=atoi(optarg);
      	if(NUM_LISTS<1) {
      		fprintf(stderr,"Error: number of threads must be greater than 0.\n");
      		close_and_exit_program(1);
      	}
      	break;
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
	        if(strcmp(optarg,"none")!=0 && strcmp(optarg,"s")!=0 && strcmp(optarg,"m")!=0) {
	       		fprintf(stderr,"Error: sync type must be s or m.\n");
	      		close_and_exit_program(1);
	        }
	        SYNC_TYPE=optarg;
	      	break;
	      case 'y':
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
	strcat(name, "list");
	strcat(name, "-");
	if(!YIELD_FLAG) strcat(name, "none");
	if(YIELD_FLAG & INSERT_YIELD)strcat(name, "i");
	if(YIELD_FLAG & DELETE_YIELD)strcat(name, "d");
	if(YIELD_FLAG & LOOKUP_YIELD)strcat(name, "l");
	strcat(name, "-");
	strcat(name, SYNC_TYPE);
	fprintf(stdout, "%s,%d,%d,%d,", name, NUM_THREADS, NUM_ITERATIONS,NUM_LISTS); 
}