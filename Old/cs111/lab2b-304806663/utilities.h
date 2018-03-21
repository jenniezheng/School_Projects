#ifndef UTILITY_INCLUDED
#define UTILITY_INCLUDED

void start_timer(struct timespec* start_time);
long long end_timer(struct timespec* start_time);

//Called when inappropriate argument recieved
void print_usage();
//this function checks for arguments
void process_args(int argc, char **argv);

void print_arguments_description();

void close_and_exit_program(int return_code);

//returns lock wait time
long long lock(int num);
void unlock(int num);

#endif