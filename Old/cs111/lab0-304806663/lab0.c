//NAME: Yunjing Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>

void printError(){
	fprintf(stderr,"Error: %s\n", strerror(errno));
}

int segfaulter(){
    char* ptr=NULL;
    for(int i=0; i<1000; i++)
    	ptr[i]='a';
}

void sighandler(int signum){
	fprintf(stderr, "Error: Segmentation fault. Fault caught.\n");
    exit(4);
}

int main (int argc, char **argv)
{
  int c;
  int segfaultFlag=0;
  int fileDes;

  while (1)
    {
      static struct option long_options[] =
        {
          {"input",     required_argument,       0, 'i'},
          {"output",  required_argument,       0, 'o'},
          {"segfault",  no_argument, 0, 's'},
          {"catch",  no_argument, 0, 'c'},
          {0, 0, 0, 0}
        };
      int option_index = 0;

      c = getopt_long (argc, argv, "io:s:c:",
                       long_options, &option_index);

      if (c == -1)
        break;

      switch (c)
        {
        case 'i':
        	fileDes = open(optarg, O_RDONLY);
			if (fileDes >= 0) {
				close(0);
				dup(fileDes);
				close(fileDes);
			}
			else {
				printError();
				fprintf(stderr, "Cannot open input file `%s'\n",optarg);
				exit(2);
			}
         	break;

        case 'o':
        	fileDes = creat(optarg, S_IRWXU);
			if (fileDes >= 0) {
				close(1);
				dup(fileDes);
				close(fileDes);
			}
			else {
				printError();
				fprintf(stderr, "Cannot create output file `%s'\n",optarg);
				exit(3);
			}
          	break;
        case 's':
          segfaultFlag=1;
          break;
        case 'c':
          signal(SIGSEGV, sighandler);
          break;
        case '?':
      	  fprintf(stderr, "Usage:\t--input=[filename]\tuse filename as input\n\t --output=[filename]\tuse filename as output file\
      	  			\n\t --segfault\t\tforce a segmentation fault\n\t --catch\t\tcatch segmentation fault with signal handler\n");
          exit(1);
        default:
          exit(1);
        }
    }

    if(segfaultFlag) segfaulter();
    char buf[101];
    int bytes;
    while(1){
    	bytes=read(0, buf, 100);
    	if(bytes==0) break;
    	bytes=write(1, buf, bytes);
    }
  
    exit(0);

}