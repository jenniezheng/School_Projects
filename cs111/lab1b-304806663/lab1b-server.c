//NAME: Yunjing Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>
#include "crypt.h"

// flags. Note crypt flag should be set in both client and server 
// for proper encryption and decryption
int DEBUG_FLAG, CRYPT_FLAG;
// port number > 1024 and client's port number = server's port number
int SOCKET_FD, NEW_SOCKET_FD, KEY_FILE_FD;
int PORT_NUMBER;
// input and output file descriptors of pipe1 and pipe2
int PIPE1_FD[2]; 
int PIPE2_FD[2];
// process id of child
int PID; 
MCRYPT ENCRYPT_FD, DECRYPT_FD;
//length of crypt key 
int KEY_LENGTH;
char* KEY;

void close_and_exit_program(int exit_code);
int read_and_write(int reader_fd, int writer_fd);

//handles SIGPIPE
void signal_handler(int num){
    if(DEBUG_FLAG)printf("SIGPIPE recieved.\r\n");
    close_and_exit_program(0);
}

//Called when inappropriate argument recieved
void print_usage(){
	fprintf(stderr, "Usage: lab1b-server [OPTION]... --port=PORT_NUMBER\
\n\rCreate server terminal. Note that server and client must use the same port.\
\n\rOptional arguments:\
\n\r\t--encryption=KEY_FILE\toptional argument to encrypt messages using the given key. Server and client must use the same key\
\n\r\t--debug\t\t\toptional argument to print debug statements\n");  
}

//this function checks for arguments
void process_args(int argc, char **argv){
	char *key_file;
	static struct option long_options[] =
	{
	  	{"port", required_argument, 0, 'p'}, 
		{"encryption", required_argument, 0, 'e'},
	    {"debug", no_argument, 0, 'd'},
	    {0, 0, 0, 0}
	};
	int c=0;
	while ((c = getopt_long(argc, argv, "p:e:d:", long_options, NULL)) != -1){
	    //no argument, break
	    if (c == -1) break;
	    switch (c)
	    {
	      case 'p': 
	      	//should be > 1024
	      	PORT_NUMBER=atoi(optarg);
	      	break;
	      case 'e': 
	      	CRYPT_FLAG=1;
	      	key_file=optarg;
	      	KEY_FILE_FD = open(key_file, O_RDONLY);
			if(KEY_FILE_FD<0){
				perror("Error: Failed to open crypt file");
				exit(FAILURE_EXIT);
			}
	      	break;
	      case 'd':
	        DEBUG_FLAG=1;
	        break;
	      case '?':
	      	print_usage();
	      	exit(EXIT_FAILURE);
	      default:
	        // should never get here. 
	      	if(DEBUG_FLAG) printf("Error: Unexpected case.\n\r");
	        exit(EXIT_FAILURE);
	    }
  	}

	if(DEBUG_FLAG){
		printf("Debugging...\n\r");
		printf("Port set to %d\n\r", PORT_NUMBER);
		if(CRYPT_FLAG) {
			printf("Crypting with key %s\n\r",key_file); 
		}
	}
	//catch no port number error
	if(!PORT_NUMBER){
		fprintf(stderr,"Error: Port number must be declared with argument --port=PORT_NUMBER\n\r");
		exit(EXIT_FAILURE);
	}
}

//set up socket for client to connect 
void set_up_connection(){
	//Referred to example from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
	struct sockaddr_in server_address, client_address;
	struct hostent* server;
	int num_bytes, client_address_length;
	if(DEBUG_FLAG) printf("\rEstablishing connection...\n\r");
	//Socket initialzation
	//Establish socket file descriptor, for TCP with internet domain
	SOCKET_FD = socket(AF_INET, SOCK_STREAM, 0);
	if(SOCKET_FD < 0) { perror("\rError: socket creation problem"); exit(FAILURE_EXIT); }
	//allow port number reuse without wait
	int allow = 1;
	if (setsockopt(SOCKET_FD, SOL_SOCKET, SO_REUSEADDR, &allow, sizeof(int)) < 0)
	    perror("Error: setsockopt(SO_REUSEADDR) failed");
	//Initialize the server address to zero 
	memset((char*) &server_address,0, sizeof(server_address));
	//assign server_address properties
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(PORT_NUMBER);
	if (bind(SOCKET_FD, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) 
         { perror("\rError: couldn't bind server address to socket file descriptor"); exit(FAILURE_EXIT);}
	listen(SOCKET_FD,1);
    client_address_length = sizeof(client_address);
    NEW_SOCKET_FD = accept(SOCKET_FD, (struct sockaddr *) &client_address, &client_address_length);
    if (NEW_SOCKET_FD < 0) 
        { perror("\rError: couldn't accept client"); exit(FAILURE_EXIT);}
	if(DEBUG_FLAG) printf("Connected to client!\n\r");
}

void report_child_exit_signal_and_status(){
  int status = 0;
  //wait for child
  if(DEBUG_FLAG) printf("Waiting for child\r\n");
  waitpid(PID,&status,0);
  //Return shell exit status
  fprintf(stderr,"\r\nSHELL EXIT SIGNAL=%d STATUS=%d\r\n", 0x007f&status, 0xff00&status);
}

//This function is called to close things and then exit
void close_and_exit_program(int exit_code){
  if(PIPE1_FD[0])close(PIPE1_FD[0]);
  if(PIPE1_FD[1])close(PIPE1_FD[1]);
  if(PIPE2_FD[0])close(PIPE2_FD[0]);
  if(PIPE2_FD[1])close(PIPE2_FD[1]);
  if(PID) {
	if(ENCRYPT_FD)deinit_crypt_fd();
	if(SOCKET_FD)close(SOCKET_FD);
	if(NEW_SOCKET_FD)close(NEW_SOCKET_FD);
	if(KEY_FILE_FD)close(KEY_FILE_FD);
	if(exit_code==0){
		//print return status only if normal exit
		report_child_exit_signal_and_status();
	}
  }
  exit(exit_code);
}

//responses
void control_D_response(){
	if(DEBUG_FLAG) printf("^D recieved.\r\n");
		if(DEBUG_FLAG) printf("Closing write end of pipe 1.\r\n");
		// close write pipe to shell
		close(PIPE1_FD[1]);
		//prevent closing twice
		PIPE1_FD[1]=0;
		while(read_and_write(PIPE2_FD[0], NEW_SOCKET_FD)>0){}
		//wait for child before exiting
		close_and_exit_program(0);
}

void control_C_response(){
	if(DEBUG_FLAG) printf("^C recieved.\r\n");
	// if entered to the keyboard, send SIGINT to shell
	if(DEBUG_FLAG) printf("Sending SIGINT to child process with pid %d.\r\n", PID);
	kill(PID,SIGINT);
}

void EOF_or_read_error_from_network_response(){
	if(DEBUG_FLAG) printf("\r\nEOF from client recieved.\r\n");
	if(DEBUG_FLAG) printf("Closing write end of pipe 1.\r\n");
	// close write pipe to shell
	close(PIPE1_FD[1]);
	//prevent closing twice
	PIPE1_FD[1]=0;
	char buffer[256];
	//await EOF
    while(read(PIPE2_FD[0], buffer,256)>0){}
	close_and_exit_program(0);
}

void EOF_or_SIGPIPE_from_shell_response(){
	if(DEBUG_FLAG)printf("\r\nEOF from child recieved.\r\n");
    close_and_exit_program(0);
}


//This read/write function is called by just the poll to send from socket 
//to child and from child to socket 
//return -1 for no bytes, 0 for no connection, or 1 for bytes remaining
int read_and_write(int reader_fd, int writer_fd){
	int buff_size=256;
	char buffer[buff_size];
	int bytes_read;
	//keep reading 
	bytes_read=read(reader_fd, buffer, buff_size);
	if(bytes_read<0) 
		return -1;
	else if(bytes_read==0) {
		//read error from network connection, likely because client exited
		if(reader_fd==NEW_SOCKET_FD){
			EOF_or_read_error_from_network_response();
		}
		//child has no more input
		else {
			EOF_or_SIGPIPE_from_shell_response();
		}
	}
	buffer[bytes_read]=0;
	//decrypt message from client
	if(reader_fd==NEW_SOCKET_FD && CRYPT_FLAG){
		decrypt(buffer,bytes_read);
	}
	else if(CRYPT_FLAG) encrypt(buffer,bytes_read);
  //encrypt message to client
  
  int i;
  for(i=0; i<bytes_read; i++){
    // if control D (signal EOF) was entered by client
    if(reader_fd==NEW_SOCKET_FD && buffer[i]==4) {
      	control_D_response();
    } 
     // if control C (signal INT) was entered by client
    else if(reader_fd==NEW_SOCKET_FD && buffer[i]==3) {
        control_C_response();
     }
    // found nothing special, output regularly.
    else if (buffer[i]=='\r') {
    	//always write as \n
    	write(writer_fd, "\n", 1);
    }
    else write(writer_fd, buffer+i, 1);
  }
  return 1;
}

//called by child after fork
void change_fd_and_create_shell(){
    if(DEBUG_FLAG) printf("Child process starting.\r\n");
    if (close(PIPE1_FD[1]) < 0) {perror("Error: child failed to close pipe end"); close_and_exit_program(1);}
    if (close(PIPE2_FD[0]) < 0) {perror("Error: child failed to close pipe end"); close_and_exit_program(1);}
    //closes STDIN_FILENO, STDOUT_FILENO, and stderr, replacing them with the correct ends of the pipes
    //Note error output and regular output are sharing the write end of pipe 2
    if (dup2(PIPE1_FD[0], STDIN_FILENO) < 0) {perror("Error: child failed to duplicate pipe"); close_and_exit_program(1);}
    if (dup2(PIPE2_FD[1], STDOUT_FILENO) < 0) {perror("Error: child failed to duplicate pipe"); close_and_exit_program(1);}
    if (dup2(PIPE2_FD[1], STDERR_FILENO) < 0) {perror("Error: child failed to duplicate pipe"); close_and_exit_program(1);}
    //close the other file descriptors
    if (close(PIPE1_FD[0]) < 0) {perror("Error: child failed to close pipe end"); close_and_exit_program(1);}
    if (close(PIPE2_FD[1]) < 0) {perror("Error: child failed to close pipe end"); close_and_exit_program(1);}
    if(execlp("/bin/bash","/bin/bash",NULL) < 0 ) {perror("Error: child failed to execute /bin/bash"); close_and_exit_program(1);}
}


//called by parent after fork
void poll_and_process_io(){
    //close child's end of the two pipes
    if(DEBUG_FLAG) printf("Parent response starting.\r\n");
    if (close(PIPE1_FD[0]) < 0) { perror("Error: failed to close pipe"); close_and_exit_program(1); }
    if (close(PIPE2_FD[1]) < 0) { perror("Error: failed to close pipe"); close_and_exit_program(1); }
    //don't close again
    PIPE1_FD[0]=0;
    PIPE2_FD[1]=0;
    struct pollfd ufds[2];
    //poll explained by http://beej.us/guide/bgnet/output/html/multipage/pollman.html
    // first poll waits for client
    ufds[0].fd = NEW_SOCKET_FD;
    
    // check for input or error events
    ufds[0].events = POLLIN | POLLHUP | POLLERR;
    // second poll waits for shell output
    ufds[1].fd = PIPE2_FD[0];
    // check for input or error events
    ufds[1].events = POLLIN | POLLHUP | POLLERR;
    
	while(1){
	      // wait for events on the two filedescriptors
	    int rv = poll(ufds, 2, 0);
	    if (rv == -1 ) {
	        perror("Error: poll event error");
	        exit(FAILURE_EXIT);
	    }
	    else {
			// check for events from shell:
			while (ufds[1].revents & POLLIN) {
				//read shell output and write to client
				read_and_write(PIPE2_FD[0], NEW_SOCKET_FD);
				poll(ufds, 2, 0);
			}
		    //check for child hangup
			if (ufds[0].revents & POLLHUP || ufds[1].revents & POLLHUP){
				if(DEBUG_FLAG) printf("Poll recieved signal POLLHUP.\r\n");
				close_and_exit_program(0);
			}
			// check for events from client:
			while (ufds[0].revents & POLLIN) {
				//read from child and write to shell input
				read_and_write(NEW_SOCKET_FD, PIPE1_FD[1]);
				poll(ufds, 2, 0);
			}
			// check for errors and hangups:
			if (ufds[0].revents & POLLERR || ufds[1].revents & POLLERR) {
				if(DEBUG_FLAG) printf("POLL recieved signal POLLERR.\r\n");
				close_and_exit_program(1);
			}
		}
  	}	
}


void create_pipes(){
	if (pipe(PIPE1_FD)< 0) { PIPE1_FD[0]=0; PIPE1_FD[1]=0; //prevent closing twice
		perror("Error: pipe initializing failed"); close_and_exit_program(1); }
	if (pipe(PIPE2_FD)< 0) { PIPE2_FD[0]=0; PIPE2_FD[1]=0; //prevent closing twice
		perror("Error: pipe initializing failed"); close_and_exit_program(1); }
}

int main (int argc, char **argv)
{
	//process the arguments
	create_pipes();
	PID=fork();
	if (PID < 0) { perror("Error: forking child failed"); close_and_exit_program(1); }
	//parent process
	else if (PID!=0){
		process_args(argc, argv);
		//sigpipe handler to respond to pipe write errors
		signal(SIGPIPE,signal_handler);
		if(CRYPT_FLAG){
			create_key();
			init_crypt_fd();
		}
		set_up_connection(); 
		poll_and_process_io();
	}
	else change_fd_and_create_shell();
}