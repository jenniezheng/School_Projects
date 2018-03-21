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

// can adjust using process arguments
int DEBUG_FLAG, YIELD_FLAG, NUM_THREADS=1, NUM_ITERATIONS=1, NUM_LISTS=1;
char* SYNC_TYPE="none";
long long total_wait_time=0;
SortedList_t** LIST;

int* SPIN_LOCK;
pthread_mutex_t* MUTEX;

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

int get_list_num(const char* string){
	unsigned long hash = 1232;
    long long c;
    while ((c = *string)){
        hash = ((hash << 4) + hash) + c; 
        string++;
    }
    return hash%NUM_LISTS;
}

long total_List_Length(long long* wait_time){
	long total_length=0;
	int list_index;
	for (list_index=0; list_index<NUM_LISTS; list_index++){
		if(wait_time) *wait_time+=lock(list_index);
		int length=SortedList_length(LIST[list_index]);
		unlock(list_index);
		if(length==-1) {
			if(DEBUG_FLAG)fprintf(stderr, "Error: corrupted list discovered during SortedList_length function\n");
			close_and_exit_program(2);
		}
	}
	return total_length;
}

void* thread_task(void* list_elem){
	long long wait_time=0;
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
		int list_num=get_list_num((list_elements+index_iteration)->key);
		wait_time+=lock(list_num);
		SortedList_insert(LIST[list_num], list_elements+index_iteration);
		unlock(list_num);
	}
	//check list length	
	total_List_Length(&wait_time);
	//lookup and delete
	for(index_iteration=0; index_iteration< NUM_ITERATIONS; index_iteration++){
		int list_num=get_list_num((list_elements+index_iteration)->key);
		wait_time+=lock(list_num);
		SortedListElement_t* element=SortedList_lookup(LIST[list_num], (list_elements+index_iteration)->key);
		unlock(list_num);
		if(element==NULL ){
			if(DEBUG_FLAG)fprintf(stderr, "Error: corrupted list discovered during SortedList_lookup function\n");
			close_and_exit_program(2);
		}
		wait_time+=lock(list_num);
		int result=SortedList_delete(element);
		unlock(list_num);
		if(result==1){
			if(DEBUG_FLAG)fprintf(stderr, "Error: corrupted list discovered during SortedList_delete function\n");
			close_and_exit_program(2);
		}
		
	}

	//update total wait time
	long long old,new;
	do {
		old=total_wait_time;
		new=old+wait_time;
	}
	while(__sync_val_compare_and_swap(&total_wait_time,old,new)!=old);
	return NULL;
}

long long thread_spawner(){
	//initialize empty list with dummy as first element

	SPIN_LOCK=calloc(NUM_LISTS,sizeof(int));
	MUTEX=calloc(NUM_LISTS,sizeof(pthread_mutex_t));
	LIST=calloc(NUM_LISTS,sizeof(SortedList_t*));
	int list_index;
	//create list dummy nodes and sync structures for lists
	for (list_index=0; list_index<NUM_LISTS; list_index++){
		pthread_mutex_init(MUTEX+list_index, NULL);
		SPIN_LOCK[list_index]=0;
		LIST[list_index]=malloc(sizeof(SortedList_t));
		LIST[list_index]->next=LIST[list_index];
		LIST[list_index]->prev=LIST[list_index];
		//dummy key always first
		LIST[list_index]->key=""; 
	}

	//create list elements
	SortedListElement_t* list_elements=calloc(NUM_ITERATIONS*NUM_THREADS, sizeof(SortedList_t));
	int element_index;
	for(element_index=0; element_index< NUM_THREADS*NUM_ITERATIONS; element_index++){
		(list_elements+element_index)->key=generate_random_string();
	}
	//create thread structures
	pthread_t* my_threads=calloc(NUM_THREADS, sizeof(pthread_t));
	struct timespec start_time;
	start_timer(&start_time);
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
	//check list length	
	long total_length=total_List_Length(NULL);
	if (total_length!=0){
		fprintf(stderr, "Error: SortedList_length did not return an ending size of zero.\n");
		close_and_exit_program(2);
	}

	long long elapsed_time=end_timer(&start_time);
	free(my_threads);
	free(list_elements);
	free(LIST);
	free(MUTEX);
	free(SPIN_LOCK);
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
	long long average_operation_time=elapsed_time/operations;
	
	long long average_wait_time=total_wait_time/operations;
	print_arguments_description();
	fprintf(stdout, "%lld,%lld,%lld,%lld\n", operations, elapsed_time, average_operation_time, average_wait_time); 

	close_and_exit_program(0);
	return 0;
}