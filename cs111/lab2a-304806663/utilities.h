#ifndef UTILITY_INCLUDED
#define UTILITY_INCLUDED

#define ADD_TYPE 0
#define LIST_TYPE 1

extern int DEBUG_FLAG, YIELD_FLAG, NUM_THREADS, NUM_ITERATIONS;
extern char* SYNC_TYPE;
extern const int PROGRAM_TYPE;
extern int SPIN_LOCK;
extern pthread_mutex_t MUTEX;

void start_timer();
long long end_timer();

//Called when inappropriate argument recieved
void print_usage();
//this function checks for arguments
void process_args(int argc, char **argv);

void close_and_exit_program(int return_code);

void print_arguments_description();

void lock();
void unlock();

#endif