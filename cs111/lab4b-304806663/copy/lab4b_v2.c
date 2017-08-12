#include "mraa.h"
#include <math.h>
#include <time.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int B = 4275;
const int R0 = 100;
const int button_pin = 3; 

int main()
{
  mraa_aio_context adc_a0;
  mraa_gpio_context gpio;

  time_t begin,end;

  uint16_t adcValue = 0;
  float adc_value_float = 0.0;

  int period = 1;
  char* filename = "log.txt"; 

  adc_a0 = mraa_aio_init(0);

  if (adc_a0 == NULL) {
    return 1;
  }

  gpio = mraa_gpio_init(button_pin); 
  mraa_gpio_dir(gpio, MRAA_GPIO_IN);

  time_t curtime;
  struct tm *loctime;

  FILE *fp; 

  for (;;) {
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
    /*
    fprintf (stdout, "The temperature is %0.2f degree Celcius\n", temp);
    fprintf(stdout, "Gpio is %d\n", button_value);
    fprintf(stdout, "%d:%d:%d \n",hour, min, sec); 
    */
    fprintf(fp, "Testing...\n");
    fprintf (fp, "The temperature is %0.2f degree Celcius\n", temp);
    fprintf(fp, "Gpio is %d\n", button_value);
    fprintf(fp, "%d:%d:%d \n",hour, min, sec);
    fclose(fp);

    /* delay a certain amount of time */ 
    sleep(period);


  }

  mraa_aio_close(adc_a0);
  return MRAA_SUCCESS;
}
