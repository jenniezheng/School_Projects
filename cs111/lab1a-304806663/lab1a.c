//NAME: Yunjing Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

//... normal execution or shutdown on ^D
int NORMAL_EXIT=0; 
//... unrecognized argument or system call failure
int FAILURE_EXIT=1; 
// whether or not --shell option was selected
static int SHELL_FLAG=0; 
static int DEBUG_FLAG=0; 
// original configuration of the terminal
struct termios ORIGINAL_CONFIGURATION; 
// input and output file descriptors of pipe1 and pipe2
int PIPE1_FD[2]; 
int PIPE2_FD[2];
// process id of child
int PID; 
//check if child created
int INITIALIZED;
// info for thread
pthread_t THREAD_ID;

void report_error_and_exit();

//This function is called to save the original terminal configuration 
//before terminal changes
void get_original_config(){
   //if the STDIN_FILENO isn't a terminal, error exit. 
  if(!isatty(0)) report_error_and_exit();
  if(tcgetattr(0, &ORIGINAL_CONFIGURATION) < 0)report_error_and_exit();
}

//This function is called to save the original terminal configuration 
//before terminal changes
void set_new_config(){
  //declare new terminal configuration
  struct termios new_config;
  //copy the original terminal configuration into the new one 
  memcpy(&new_config, &ORIGINAL_CONFIGURATION, sizeof(struct termios));
  new_config.c_lflag = 0;    
  /* leave only lower 7 bits  */
  new_config.c_iflag = ISTRIP;  
  /* no processing  */
  new_config.c_oflag = 0;    
  // switch to new configuration
  if(tcsetattr(STDIN_FILENO,TCSANOW, &new_config) < 0) report_error_and_exit();
}

//This function is called upon system exit from the atexit declaration 
//to restore terminal configuration before process exit
void restore_original_config(){
  if (tcsetattr(STDIN_FILENO, TCSANOW, &ORIGINAL_CONFIGURATION) < 0)
    fprintf(stderr,"Restore config error: %s\n", strerror(errno));
}

//This function is called when system calls fail.
//It sends the appropriate error message and then exits
void report_error_and_exit(){
  fprintf(stderr,"Error: %s\n", strerror(errno));
  exit(EXIT_FAILURE);
}

void wait_for_child_and_exit(){
  int status = 0;
  //wait for child
  waitpid(PID, &status,0);
  //Return shell exit status
  printf("\r\nSHELL EXIT SIGNAL=%d STATUS=%d\r\n",WTERMSIG(status), WEXITSTATUS(status));
  exit(0);
}


//This read/write function is called by just the poll to send STDIN_FILENO to STDOUT_FILENO.
//When the SHELL_FLAG is set, the poll also sends STDIN_FILENO to the 
//child's bash program and sends the bash output to STDOUT_FILENO 
//return STDIN_FILENO for no more bytes or 1 for bytes remaining
int read_and_write(int reader_fd, int writer_fd){
  int buff_size=256;
  char buffer[buff_size];
  int bytes_read;
  //keep reading 
  bytes_read=read(reader_fd, buffer, buff_size);
  if(bytes_read<0) report_error_and_exit();
  else if(bytes_read==0) return 0;
  int i;
  for(i=0; i<bytes_read; i++){
    // if control D (signal EOF) was entered
    if(buffer[i]==4) {
      if(DEBUG_FLAG) printf("^D recieved.\r\n");
      // if shell flag and reading from keyboard
      if(SHELL_FLAG) {
        // close write pipe to shell
        if (close(PIPE1_FD[1]) < 0) report_error_and_exit();
        //continue processing input from shell while there's input
        printf("\r\n");
        while(read_and_write(PIPE2_FD[0], STDOUT_FILENO)){ }
        if (close(PIPE2_FD[0]) < 0) report_error_and_exit();
        wait_for_child_and_exit();
       }
      //may exit now
      exit(0);
      } 
     // if control C (signal INT) was entered
    else  if(buffer[i]==3) {
        if(DEBUG_FLAG) printf("^C recieved.\r\n");
        // if entered to the keyboard, send SIGINT to child
        if(SHELL_FLAG) {
          kill(PID,SIGINT);
          wait_for_child_and_exit();
        }
        exit(0);
     }
    // If carriage return or newline was entered, rewrite as \r\n.
    else if (buffer[i]=='\r' || buffer[i]=='\n') {
        if(write(writer_fd, "\r\n", 2)<0) report_error_and_exit();
        // if SHELL_FLAG set and reading from keyboard, write \n to child process 
        if(SHELL_FLAG && reader_fd==STDIN_FILENO)  {
          if(write(PIPE1_FD[1], "\n", 1)<0) report_error_and_exit();
        }
    }
    // found nothing special, output regularly.
    else {
        if(write(writer_fd, buffer+i, 1)<0) report_error_and_exit();
        // if SHELL_FLAG set and reading from keyboard, write character to child process 
        if(SHELL_FLAG && reader_fd==STDIN_FILENO)  
          if(write(PIPE1_FD[1], buffer+i, 1)<0) report_error_and_exit(); 
    }
  }
}


//This function is called if the SHELL_FLAG is set
void regular_response(){
    while(read_and_write(STDIN_FILENO,STDOUT_FILENO)){  }
    exit(0);
}

//called by child after fork
void child_response(){
 
    if(DEBUG_FLAG) printf("Child process starting.\r\n");
    if (close(PIPE1_FD[1]) < 0) report_error_and_exit();
    if (close(PIPE2_FD[0]) < 0) report_error_and_exit();
    //closes STDIN_FILENO, STDOUT_FILENO, and stderr, replacing them with the correct ends of the pipes
    //Note error output and regular output are using the end of the same pipe
    if (dup2(PIPE1_FD[0], 0) < 0) report_error_and_exit();
    if (dup2(PIPE2_FD[1], 1) < 0) report_error_and_exit();
    if (dup2(PIPE2_FD[1], 2) < 0) report_error_and_exit();
    //close the other file descriptors
    if (close(PIPE1_FD[0]) < 0) report_error_and_exit();
    if (close(PIPE2_FD[1]) < 0) report_error_and_exit();
    //execute bash 
    if(DEBUG_FLAG) printf("About to execute bash.\r\n");
    if(execlp("/bin/bash","/bin/bash",NULL) < 0 ) report_error_and_exit();

}


//called by parent after fork
void parent_response(){
 
    //close child's end of the two pipes
    if(DEBUG_FLAG) printf("Parent response starting.\r\n");

    if (close(PIPE1_FD[0]) < 0) report_error_and_exit();
    if (close(PIPE2_FD[1]) < 0) report_error_and_exit();

    struct pollfd ufds[2];
    //poll explained by http://beej.us/guide/bgnet/output/html/multipage/pollman.html
    // first poll waits for keyboard
    ufds[0].fd = 0;
    // check for input or error events
    ufds[0].events = POLLIN | POLLHUP | POLLERR;
    // second poll waits for shell output
    ufds[1].fd = PIPE2_FD[0];
    // check for input or error events
    ufds[1].events = POLLIN | POLLHUP | POLLERR;
 

    while(1){
      // wait for events on the two filedescriptors, no timeout
    int rv = poll(ufds, 2, 0);
    if (rv == -1 ) 
        report_error_and_exit();
    else {
      // check for events on keyboard:
      if (ufds[0].revents & POLLIN) {
          //read and write keyboard to shell input
          read_and_write(STDIN_FILENO, STDOUT_FILENO); 
      }
      if (ufds[1].revents & POLLIN) {
          //read and write shell output to output
          read_and_write(PIPE2_FD[0], STDOUT_FILENO); 
      }
      // check for errors and hangups:
      if (ufds[0].revents & POLLERR || ufds[1].revents & POLLERR) 
          report_error_and_exit();
      if (ufds[0].revents & POLLHUP || ufds[1].revents & POLLHUP) {
          if (close(PIPE2_FD[1]) < 0) report_error_and_exit();
          if (close(PIPE1_FD[0]) < 0) report_error_and_exit();
          //alert the shell with SIGHUP
          kill(PID,SIGHUP);
          wait_for_child_and_exit();
      }
    }
  }
 
}

//This function is called if the SHELL_FLAG is set
void shell_response(){
  //initialize the two pipes
  if (pipe(PIPE1_FD)< 0) report_error_and_exit();
  if (pipe(PIPE2_FD)< 0) report_error_and_exit();

  //fork into two processes
  PID=fork();
  if (PID < 0) report_error_and_exit();
  //parent process
  else if (PID!=0){
    parent_response();
  }
  else child_response();
}


//this function checks for arguments
void process_args(int argc, char **argv){
  //detect argument --shell, --debug
  while (1)
  {
    static struct option long_options[] =
      {
        {"shell", no_argument, &SHELL_FLAG, 1},
        {"debug", no_argument, &DEBUG_FLAG, 1},
        {0, 0, 0, 0}
      };
    int option_index = 0;
    char c = getopt_long(argc, argv, "s:",
                     long_options, &option_index);
    //no argument, break
    if (c == -1) break;
    switch (c)
    {
      case 0:
        // flag was set, done 
        if (long_options[option_index].flag != 0) break;
      case '?':
        // inappropriate argument. print usage and then exit. 
        fprintf(stderr, "Usage:\t--shell\t\toptional argument to create a bash shell\n\t--debug\t\toptional argument to debug script\n");
        exit(EXIT_FAILURE);
      default:
        // should never get here. 
        exit(EXIT_FAILURE);
      }
  }
}

int main (int argc, char **argv)
{
  //set the shell flag if --shell option was selected
  process_args(argc, argv);
  //get the original configuration of the terminal
  get_original_config();
  //change the configuration to character at a time, no echo
  set_new_config();
  //restore at exit, always
  atexit(restore_original_config);
  //shell flag set!
  if(SHELL_FLAG) shell_response();
  //read and write regularly from STDIN_FILENO to STDOUT_FILENO
  else regular_response();
  exit(0);
}