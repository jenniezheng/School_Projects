#include "mraa.h"
#include <math.h>
#include <time.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>

const int B = 4275;
const int R0 = 100;
const int button_pin = 3; 
FILE *fp;
int make_reports=1; 
int celcius=1; // default: celcius; alt: F 

void shutdown()
{
  // log time and SHUTDOWN 
  time_t curtime;
  curtime = time (NULL);
  struct tm *tm_struct = localtime (&curtime);
  int hour = tm_struct -> tm_hour;
  int min = tm_struct -> tm_min;
  int sec = tm_struct -> tm_sec;

  fprintf(fp, "%d:%d:%d SHUTDOWN\n",hour, min, sec);
  exit(0); 
}

int main()
{
  mraa_aio_context adc_a0;
  mraa_gpio_context gpio;
  uint16_t adcValue = 0;
  float adc_value_float = 0.0;
  int period = 1;
  char* filename = "log.txt"; 
  adc_a0 = mraa_aio_init(0);

  if (adc_a0 == NULL) {
    return 1;
  }

  char* buffer;
  size_t bufsize = 32;
  size_t characters; 
  buffer = (char *)malloc(bufsize * sizeof(char));

  gpio = mraa_gpio_init(button_pin); 
  mraa_gpio_dir(gpio, MRAA_GPIO_IN);

  time_t curtime; 
  
  time_t start, end;
  double elapsed;

  struct pollfd fds;
  int ret; 
  fds.fd = 0; /* this is STDIN */
  fds.events = POLLIN;

  while(!mraa_gpio_read(gpio)){ // while button is not pressed 

    /* poll for input */ 
    ret = poll(&fds, 1, 0);
    if(ret == 1)
      printf("Yep\n");
    else if(ret == 0)
      printf("No\n");
    else
      printf("Error\n");

    /* read input if there */
    if(fds.revents & POLLIN)
      {
	characters = getline(&buffer,&bufsize,stdin);
	printf("You typed: %s \n",buffer);

	/*
	if(strcmp(buffer, "OFF"))
	  {
	    fprintf(fp, "OFF\n");
	    //	    shutdown();
	  }
	else if(strcmp(buffer, "STOP"))
	  {
	    make_reports = 0;
	    fprintf(fp,"STOP\n");
	    fprintf(stderr, "...STOP\n");
	  }
	else if(strcmp("START", buffer))
	  {
	    make_reports = 1 ;
	    fprintf(stderr, "...START\n"); 
	  }
	else if(strcmp(buffer, "SCALE=F"))
	  {
	    celcius=0;
	    fprintf(fp, "SCALE=F\n");
	  }
	else if(strcmp(buffer, "SCALE=C"))
	  {
	    celcius=1;
	    fprintf(fp, "SCALE=C\n");
	  }
	else if(strcmp(buffer, "PERIOD=")) // TODO
	  {

	  }
	else
	  {
	    fprintf(stderr, "Error: input option not valid");
	    //      exit(1);
	  }
	*/ 
      } // end of poll if 

    fp = fopen("log.txt", "a");
    /* Calculate temperature reading */ 
    adcValue = mraa_aio_read(adc_a0);
    float R;
    R = 1023.0/((float)adcValue)-1.0;
    R = 100000.0*R;
    float temp  = 1.0/(log(R/100000.0)/B+1/298.15)-273.15;
    
    /* button reading */ 
    int button_value = mraa_gpio_read(gpio); 
    
    /* Local Time */ 
    
    curtime = time (NULL);
    struct tm *tm_struct = localtime (&curtime);
    int hour = tm_struct -> tm_hour; 
    int min = tm_struct -> tm_min; 
    int sec = tm_struct -> tm_sec; 
    

    /* print logs  */
    
    fprintf (stdout, "The temperature is %0.2f degree Celcius\n", temp);
    fprintf(stdout, "Gpio is %d\n", button_value);
    fprintf(stdout, "%d:%d:%d \n",hour, min, sec); 
    
    fprintf(fp, "Testing...\n");
    fprintf (fp, "The temperature is %0.2f degree Celcius\n", temp);
    fprintf(fp, "Gpio is %d\n", button_value);
    fprintf(fp, "%d:%d:%d \n",hour, min, sec);
    fclose(fp);
    
    /* Delay time */ 
    time(&start);
    do {
      time(&end);
      elapsed = difftime(end, start);
    } while(elapsed < period);
  
  } // end of infinite for-loop 

  mraa_aio_close(adc_a0);

  //  shutdown(); 

  return MRAA_SUCCESS;
}
