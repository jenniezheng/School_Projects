//NAME: Jennie Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663

//for running on my own computer
#ifdef __amd64 
#define mraa_aio_context char*
#define mraa_aio_read(X) 250+rand()%100
#define mraa_aio_init(X) "No Init Needed"
#define mraa_aio_close(TEMP_SENSOR) return
#endif

#ifndef __amd64 
#include <mraa/aio.h>
#endif

#include "connect.h"

#include <getopt.h>
#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h> 
#include <unistd.h>

mraa_aio_context TEMP_SENSOR;
int LOG_FD;
int DEBUG_FLAG;
int PERIOD=1;
int SCALE_FARENHEIT=1;//1 when farenheit, 0 when celcius
int LOG=0;
int STOP=0;//1 when stop, 0 when start
int ID=918273645; //just a random number
char* HOST="lever.cs.ucla.edu"; //default host
int PORT_NUM; //required argument
int SOCKET_FD;
extern char* CONNECT_TYPE;

/*STARTING*/
//takes in command line arguments and sets starting settings
void process_args(int argc, char **argv);
//initializes the temperature sensor
void init_sensors();
//closes the temperature sensor

/*ENDING*/
void close_sensors();
//closes and exits the program
void close_and_exit_program(int exit_code);
//shuts down program
void shut_down();

/*REPORTING TEMP*/
//obtains the temperature and reports it to stdout, as well as log if log is turned on
void report(); 
//takes in a raw temp and returns it in either celcius or farenheit, depending on settings
float convert_temp(int raw_value);
//reads the temperature and returns its raw value
float read_temp();
//accepts command ending in /0 and processes that single command

/*PROCESSING COMMANDS*/
void process_command(char* command);
//splits buff into lines, processing finished lines and leaving remainder in buff
void process_lines(char* buff);

/*TIME*/
//returns the local time in format Hour:Min:Sec
char* get_current_local_time();
//if places tens place in first pos and ones in second. 
void place_time_in_slot(char* slots, int time);

/*PRINTING*/
//prints ID
void printID();
//print general
void print_general(char* msg);
//prints how to use this program
void print_usage();

/*POLLING*/
void run();


void process_lines(char* buff){
	char* current_line=buff;
	while(1)
    {
      char* next_line = strchr(current_line, '\n');
      if (next_line) {
	      *next_line = '\0';  // terminate the line
	      process_command(current_line);
	      current_line=next_line+1;
	      if(!current_line){
	      	buff[0]='\0';  // clear the buffer, no more commands
	      	return;
	      }
	  }
      else{ //unfinished command
      	int current_line_length=strlen(current_line); 
      	if (current_line_length>=50){
      		fprintf(stderr, "Error! Command too long. Discarding command.");
      		buff[0]='\0'; // clear the buffer, discard command
      	}
      	else{
      		strcpy(buff, current_line);
      		buff[current_line_length]='\0';  // end the buffer
      	}
      	return;
      }
   }
}

void shut_down(){
	char* time=get_current_local_time();
	char* msg=malloc(sizeof(char)*100);
	sprintf(msg,"%s SHUTDOWN\n",time);
	print_general(msg);
	free(msg);
	free(time);
	close_and_exit_program(0);
}

void process_command(char* command){
	if(LOG) {
		write(LOG_FD,command,strlen(command));
		write(LOG_FD,"\n",1);
	}
	if(DEBUG_FLAG) {
		write(0,command,strlen(command));
		write(0,"\n",1);
	} 
	if(strcmp(command,"OFF")==0) shut_down();
	else if(strcmp(command,"SCALE=F")==0) SCALE_FARENHEIT=1;
	else if (strcmp(command,"SCALE=C")==0) SCALE_FARENHEIT=0;
	else if (strcmp(command,"STOP")==0) STOP=1;
	else if (strcmp(command,"START")==0) STOP=0;
	else if(strncmp(command, "PERIOD=",7)==0 && command[7]!=0){
		PERIOD=atoi(command+7);
	}
	else fprintf(stderr,"Error: Unrecognized command %s!\n", command);	
}



void report(){
	float temp=read_temp();
	char* time=get_current_local_time();
	char* msg=malloc(sizeof(char)*100);
	sprintf(msg, "%s %0.1f\n",time, temp);
	print_general(msg);
	free(msg);
	free(time);
}

void printID(){
	char* msg=malloc(sizeof(char)*100);
	sprintf(msg, "ID=%d\n",ID);
	print_general(msg);
}

void print_general(char* msg){
	writeToServer(msg, strlen(msg));
	if(LOG) write(LOG_FD, msg,strlen(msg));
	if(DEBUG_FLAG) write(0, msg,strlen(msg));
}

void place_time_in_slot(char* slots, int time){
	slots[0]='0'+time/10;
	slots[1]='0'+time%10;
}

char* get_current_local_time(){
	time_t current_time;
	struct tm *local_time;
	//obtain the current time
	current_time = time(NULL);
	//convert that into local time
	local_time = localtime (&current_time);
	int hour = local_time -> tm_hour; 
	int min = local_time -> tm_min; 
	int sec = local_time -> tm_sec;
	char* slots=malloc(sizeof(char)*9);
	place_time_in_slot(slots,hour);
	place_time_in_slot(slots+3,min);
	place_time_in_slot(slots+6,sec);
	slots[2]=':';
	slots[5]=':';
	slots[8]='\0';
	return slots;

}

float convert_temp(int raw_temp_value){
	const float B = 4275;               // B value of the thermistor
	const float R0 = 100000.0;            // R0 = 100k
	float R = 1023.0/(float)raw_temp_value-1.0;
	R = R0*R;
	float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; 
	return temperature;
}

float read_temp(){
	int value=mraa_aio_read(TEMP_SENSOR);
	float celcius_temp=convert_temp(value);
	if(!SCALE_FARENHEIT)
		return celcius_temp;
	else{
		float fahrenheit_temp=celcius_temp*9/5 + 32;
		return fahrenheit_temp;
	}
}

void init_sensors(){
	TEMP_SENSOR = mraa_aio_init(0) ;
	if (TEMP_SENSOR == NULL) {
    	fprintf(stderr,"Error finding temp_sensor!\n");
    	close_and_exit_program(2);
  	}
}

void close_sensors(){
	mraa_aio_close(TEMP_SENSOR);
}

void close_and_exit_program(int exit_code){
	close_sensors();
	close(SOCKET_FD);
	exit(exit_code);
}

void print_usage(){
	fprintf(stderr,"Usage:	./lab4c_%s [OPTION]... PORT_NUM\n",CONNECT_TYPE);
	fprintf(stderr,"This program runs on it intel edison and assumes that a temperature sensor is plugged \
in to A0. It will connect with %s and continuously check the temperature and process commands \
like STOP, START, and SCALE=[C or F] until the command OFF is called. \n\n",CONNECT_TYPE);
	fprintf(stderr,"Optional Arguments: \n");
	fprintf(stderr,"--id=[ID]	optional argument to specify an id\n");
	fprintf(stderr,"--host=[HOST]	optional argument to specify the host to connect to.\n");
	fprintf(stderr,"--log=[FILENAME]	optional argument to log temperature to specified file.\n");
}

//this function checks for arguments
void process_args(int argc, char **argv){
	int num_optional_args=0;

	//process optional arguments
	static struct option long_options_list[] =
	{
		{"id", required_argument, 0, 'i'}, 
		{"host", required_argument, 0, 'h'},
		{"log", required_argument, 0, 'l'},
		{"debug", no_argument, 0, 'd'},
		{0, 0, 0, 0}
	};

	int c=0;
	while ((c = getopt_long(argc, argv, "ihld:", long_options_list, NULL)) != -1){
	    //no argument, break
		if (c == -1) break;
		num_optional_args++;
		switch (c)
		{
			case 'i': 
				ID=atoi(optarg);
			break;
			case 'h': 
				HOST=optarg;
			break;
			case 'l': 
				if(DEBUG_FLAG) printf("logfile is %s\n",optarg);
				LOG_FD=open(optarg, O_CREAT|O_APPEND|O_WRONLY, 0666);
				LOG=1;
			break;
			case 'd':
				DEBUG_FLAG=1;
			break;
			case '?':
				fprintf(stderr,"Error: unrecognized argument\n");
				print_usage();
				close_and_exit_program(1);
			default:
		        // should never get here. 
				close_and_exit_program(1);
		}
	}
	//Checking for port number
  	if(argc<num_optional_args+2) {
  		fprintf(stderr,"Error: Port number required\n");
  		print_usage();
  		close_and_exit_program(1);
  	}

  	PORT_NUM=atoi(argv[num_optional_args+1]);
  	if(PORT_NUM<=0) {
  		fprintf(stderr,"Error: Invalid port number\n");
  		print_usage();
  		close_and_exit_program(1);
  	}
}


void run(){
	const int READ_BUFF_SIZE=64,SAVE_BUFF_SIZE=128; 
	struct pollfd fds;
    time_t last_temp_read, current_time;
    double elapsed_time;
	char read_buffer[READ_BUFF_SIZE];
	char save_buffer[SAVE_BUFF_SIZE];
	memset(save_buffer,0,SAVE_BUFF_SIZE); //clear save buffer
    fds.fd = SOCKET_FD; //poll socket
    fds.events = POLLIN;
    time(&last_temp_read);
	while(1){
		int ret = poll(&fds, 1, 0);
  		if(ret == 1){
  			int num_read=readFromServer(read_buffer,READ_BUFF_SIZE-1);
  			read_buffer[num_read]=0; //end buffer
  			strcat(save_buffer, read_buffer); //add to save buff
  			process_lines(save_buffer);
  		}
     	else if(ret!=0){
     		fprintf(stderr,"Error encountered when polling\n");
     		close_and_exit_program(2);
     	}
     	if(STOP) continue;
        time(&current_time);
        elapsed_time=difftime(current_time,last_temp_read);
        if(elapsed_time>=PERIOD){
        	last_temp_read=current_time;
        	report();
        }
	}
}

int main(int argc, char **argv){
	process_args(argc, argv);
	init_sensors();
	if(DEBUG_FLAG) printf("Establishing %s connection to port %d with host %s...\n",CONNECT_TYPE,PORT_NUM,HOST);
	connectMe(PORT_NUM, HOST);
	if(DEBUG_FLAG) printf("Connected successfully. Socket fd is %d.\n",SOCKET_FD);
	printID();
    report(); //report initially
	run();
	return 0;
}




