//NAME: Jennie Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663

#include <getopt.h>
#include <math.h>
#include <mraa/aio.h>
#include <mraa/gpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <time.h> 
#include <unistd.h>

//declare button and tempsensor
mraa_gpio_context BUTTON;
mraa_aio_context TEMP_SENSOR;
FILE* LOG_FILE;
int DEBUG_FLAG;
int PERIOD=1;
int SCALE_FARENHEIT=1;//1 when farenheit, 0 when celcius
int LOG=0;
int STOP=0;//1 when stop, 0 when start

float convert_temp(int value);
float read_temp();
int read_button();
void init_sensors();
void close_sensors();
void close_and_exit_program(int exit_code);
void process_args(int argc, char **argv);
void print_usage();
void report(); 
void process_command(char* command);
void process_lines(char* buff);
struct tm* get_current_local_time();

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

void process_command(char* command){
	int recognized_argument=1;
	if(strcmp(command,"OFF")==0) {
		if(LOG){
			struct tm* local_time=get_current_local_time();
			int hour = local_time -> tm_hour; 
		    int min = local_time -> tm_min; 
		    int sec = local_time -> tm_sec; 
		    fprintf(LOG_FILE, "%d:%d:%d SHUTDOWN\n",hour,min,sec);
		}
		close_and_exit_program(0);
	}
	if(strcmp(command,"SCALE=F")==0) SCALE_FARENHEIT=1;
	else if (strcmp(command,"SCALE=C")==0) SCALE_FARENHEIT=0;
	else if (strcmp(command,"STOP")==0) STOP=1;
	else if (strcmp(command,"START")==0) STOP=0;
	else if(strncmp(command, "PERIOD=",7)==0 && command[7]!=0){
		PERIOD=atoi(command+7);
	}
	else recognized_argument=0;
	if(recognized_argument) {
		if(LOG) fprintf(LOG_FILE,"%s\n",command);
	}
	else {
		printf("Error: Unrecognized command %s!\n", command);
	}
}


void report(){
	float temp=read_temp();
	struct tm* local_time=get_current_local_time();
	int hour = local_time -> tm_hour; 
    int min = local_time -> tm_min; 
    int sec = local_time -> tm_sec; 
	fprintf(stdout, "%d:%d:%d %0.1f\n",hour,min,sec, temp);
	if(LOG) fprintf(LOG_FILE, "%d:%d:%d %0.1f\n",hour,min,sec, temp);
}

struct tm* get_current_local_time(){
	time_t current_time;
	struct tm *local_time;
	//obtain the current time
	current_time = time(NULL);
	//convert that into local time
	local_time = localtime (&current_time);
	return local_time;
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
	uint16_t value=mraa_aio_read(TEMP_SENSOR);
	float celcius_temp=convert_temp(value);
	if(!SCALE_FARENHEIT)
		return celcius_temp;
	else{
		float fahrenheit_temp=celcius_temp*9/5 + 32;
		return fahrenheit_temp;
	}
}

int read_button(){
	int button_value= mraa_gpio_read(BUTTON);
	return button_value;
}

void init_sensors(){
	TEMP_SENSOR=mraa_aio_init(0);
	BUTTON=mraa_gpio_init(3);
	//write 0 to button value
	if (TEMP_SENSOR == NULL) {
    	fprintf(stderr,"Error finding temp_sensor!\n");
    	close_and_exit_program(1);
  	}

	mraa_gpio_dir(BUTTON,MRAA_GPIO_IN);
}

void close_sensors(){
	mraa_gpio_close(BUTTON);
	mraa_aio_close(TEMP_SENSOR);
}

void close_and_exit_program(int exit_code){
	close_sensors();
	exit(exit_code);
}

void print_usage(){
	printf("Usage:	This program assumes that a temperature sensor is plugged \
in to A0 and a button is plugged into D3. It will continuously check the \
temperature and process commands like STOP, START, and SCALE=[C or F] until \
the button is pressed. \n\n");
	printf("--scale=[SCALE_TYPE]	optional argument to change scale type to farenheit (F) or celcius (C).\n");
	printf("--period=[SECONDS]	optional argument to adjust wait time in seconds.\n");
	printf("--log=[FILENAME]	optional argument to log temperature to specified file.\n");
}

//this function checks for arguments
void process_args(int argc, char **argv){
	static struct option long_options_list[] =
	{
		{"scale", required_argument, 0, 's'}, 
		{"log", required_argument, 0, 'l'},
		{"period", required_argument, 0, 'p'},
		{"debug", no_argument, 0, 'd'},
		{0, 0, 0, 0}
	};

	int c=0;
	while ((c = getopt_long(argc, argv, "s:l:p:d:", long_options_list, NULL)) != -1){
	    //no argument, break
		if (c == -1) break;
		switch (c)
		{
			case 's': 
			if(strcmp(optarg,"C")==0){
				SCALE_FARENHEIT=0;
			}
			else if(strcmp(optarg,"F")==0){
				SCALE_FARENHEIT=1;
			}
			else{
				fprintf(stderr,"Error: scale type must be C or F.\n");
				close_and_exit_program(1);
			}
			break;
			case 'l': 
			if(DEBUG_FLAG) printf("logfile is %s\n",optarg);
			LOG_FILE=fopen(optarg,"a");
			if(LOG_FILE==NULL) {
				fprintf(stderr,"Error: failed to open log file %s.\n", optarg);
				close_and_exit_program(1);
			}
			LOG=1;
			break;
			case 'p':
			PERIOD=atoi(optarg);
			if(PERIOD<1){
				fprintf(stderr,"Error: period value must be at least 1\n");
				close_and_exit_program(1);
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
			close_and_exit_program(1);
		}
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
    fds.fd = 0; //poll stdin
    fds.events = POLLIN;
    report();
    time(&last_temp_read);
    int button=read_button();
    //////
    //int what_time=0;
    /////
	while(!button){
		int ret = poll(&fds, 1, 0);
  		if(ret == 1){
  			int num_read=read(0, read_buffer,READ_BUFF_SIZE-1);
  			/*
			char* test;
  			if(what_time==0)
  				test="STOP\nSTART\nPE";
  			else if(what_time==1){
  				 test="RIOD=3\nSCAL";
  			}
  			else {
  				test="E=F\nOFF\n";
  			}
  			num_read=strlen(test);
  			what_time++;
  			memcpy(read_buffer,test,strlen(test));
  			*/

  			read_buffer[num_read]=0; //end buffer
  			strcat(save_buffer, read_buffer); //add to save buff
  			process_lines(save_buffer);
  		}
     	else if(ret!=0){
     		fprintf(stderr,"Error encountered when polling\n");
     		close_and_exit_program(1);
     	}
     	if(STOP) continue;
        time(&current_time);
        elapsed_time=difftime(current_time,last_temp_read);
        if(elapsed_time>PERIOD){
        	last_temp_read=current_time;
        	report();
        }
		button=read_button();
	}
}

int main(int argc, char **argv){
	process_args(argc, argv);
	init_sensors();
	run();
	return 0;
}




