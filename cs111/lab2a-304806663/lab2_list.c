//NAME: Yunjing Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663

#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utilities.h"
#include "SortedList.h"

const int PROGRAM_TYPE=LIST_TYPE;
// can adjust using process arguments
int DEBUG_FLAG, YIELD_FLAG, NUM_THREADS=1, NUM_ITERATIONS=1;
char* SYNC_TYPE="none";
SortedList_t* LIST;

int SPIN_LOCK;
pthread_mutex_t MUTEX = PTHREAD_MUTEX_INITIALIZER;

void signal_handler(int signal_num){
	fprintf(stderr,"Error: Segmentation fault.\n");
	close_and_exit_program(2);
}

char* generate_random_string() {
	const int length=20;
    static char character_set[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    char* random_string=malloc(sizeof(char)*(length+1));
    int i=0;      
	for (i=0;i < length; i++) {            
	    int index = rand() % (int)(sizeof(character_set) -1);
	    random_string[i] = character_set[index];
	}
	random_string[length] = '\0';
    return random_string;
}

void* thread_task(void* list_elem){
	/*
	each thread:
    starts with a set of pre-allocated and initialized elements (--iterations=#)
    inserts them all into a (single shared-by-all-threads) list
    gets the list length
    looks up and deletes each of the keys it had previously inserted
    exits to re-join the parent thread
	*/
	//inserts elements into list
	SortedListElement_t* list_elements=(SortedListElement_t*) list_elem;
	int index_iteration;
	for(index_iteration=0; index_iteration< NUM_ITERATIONS; index_iteration++){
		lock();
		SortedList_insert(LIST, list_elements+index_iteration);
		unlock();
	}
	//check list length	
	lock();
	if(SortedList_length(LIST)==-1) {
			if(DEBUG_FLAG)fprintf(stderr, "Error: corrupted list discovered during SortedList_length function\n");
			close_and_exit_program(2);
	}
	unlock();
	//lookup and delete
	for(index_iteration=0; index_iteration< NUM_ITERATIONS; index_iteration++){
		lock();
		SortedListElement_t* element=SortedList_lookup(LIST, (list_elements+index_iteration)->key);
		if(element==NULL ){
			if(DEBUG_FLAG)fprintf(stderr, "Error: corrupted list discovered during SortedList_lookup function\n");
			close_and_exit_program(2);
		}
		int result=SortedList_delete(element);
		if(result==1){
			if(DEBUG_FLAG)fprintf(stderr, "Error: corrupted list discovered during SortedList_delete function\n");
			close_and_exit_program(2);
		}
		unlock();
	}
	return NULL;
}

long long thread_spawner(){
	//initialize empty list with dummy as first element
	LIST=malloc(sizeof(SortedList_t));
	LIST->next=LIST;
	LIST->prev=LIST;
	//dummy key always first
	LIST->key=""; 
	//create list elements
	SortedListElement_t* list_elements=calloc(NUM_ITERATIONS*NUM_THREADS, sizeof(SortedList_t));
	int element_index;
	for(element_index=0; element_index< NUM_THREADS*NUM_ITERATIONS; element_index++){
		(list_elements+element_index)->key=generate_random_string();
	}
	//create thread structures
	pthread_t* my_threads=calloc(NUM_THREADS, sizeof(pthread_t));
	
	/*
	if(strcmp(SYNC_TYPE,"none")==0) params.func=add_none;
	else if (strcmp(SYNC_TYPE,"m")==0)params.func=add_m;
	else if (strcmp(SYNC_TYPE,"s")==0)params.func=add_s;
	pthread_t* my_threads=calloc(NUM_THREADS, sizeof(pthread_t));
	*/
	start_timer();
	int index_thread;
	for(index_thread=0; index_thread< NUM_THREADS; index_thread++){
		if(pthread_create(my_threads+index_thread, NULL, thread_task, (list_elements+(NUM_ITERATIONS*index_thread)))) {
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
	if(SortedList_length(LIST)!=0) {
		fprintf(stderr, "Error: SortedList_length did not return an ending size of zero.\n");
		if(DEBUG_FLAG) printf("Ending size = %d\n",SortedList_length(LIST));
		close_and_exit_program(2);
	}
	long long elapsed_time=end_timer();
	free(my_threads);
	free(list_elements);
	free(LIST);
	return elapsed_time;
}


int main (int argc, char **argv)
{
	process_args(argc, argv);
	signal(SIGSEGV, signal_handler);
	//set random seed for generating keys
	srand(time(NULL));
	long long elapsed_time=thread_spawner();
	long long operations=NUM_THREADS*NUM_ITERATIONS*3;
	/*
	Printing...
	the name of the test, which is of the form: list-yieldopts-syncopts: where
	    yieldopts = {none, i,d,l,id,il,dl,idl}
	    syncopts = {none,s,m}
	the number of threads (from --threads=)
	the number of iterations (from --iterations=)
	the number of lists (always 1 in this project)
	the total number of operations performed: threads x iterations x 3 (insert + lookup + delete)
	the total run time (in nanoseconds) for all threads
	the average time per operation (in nanoseconds).
	*/
	print_arguments_description();
	fprintf(stdout, "1,%lld, %lld, %lld\n", operations, elapsed_time, elapsed_time/operations); 
	close_and_exit_program(0);
	return 0;
}