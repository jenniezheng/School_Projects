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
int LOG_FLAG, DEBUG_FLAG, CRYPT_FLAG;
// port number > 1024 and client's port number = server's port number
int LOG_FILE_FD, SOCKET_FD, KEY_FILE_FD;

int PORT_NUMBER;
char *HOST_NAME="localhost";  
// original configuration of the terminal
struct termios ORIGINAL_CONFIGURATION; 

MCRYPT ENCRYPT_FD, DECRYPT_FD;
//length of crypt key 
int KEY_LENGTH;

char* KEY;

//This function closes files and then exits
void close_and_exit_program(int exit_code){
	if(LOG_FILE_FD)close(LOG_FILE_FD);
	if(SOCKET_FD)close(SOCKET_FD);
	if(KEY_FILE_FD)close(KEY_FILE_FD);
	if(ENCRYPT_FD)deinit_crypt_fd();
	exit(exit_code);
}

//for sigpipes
void signal_handler(int num){
	if(DEBUG_FLAG)printf("\r\nSignal %d recieved. Exiting with return code of 0\r\n",num);
	close_and_exit_program(0); 
}

//Called when inappropriate argument recieved
void print_usage(){
	fprintf(stderr, "Usage: lab1b-client [OPTION]... --port=PORT_NUMBER\
\n\rCreate client terminal to connect to server. Note that server and client must use the same port.\
\n\rOptional arguments:\
\n\r\t--log=FILE_NAME\t\toptional argument to record data sent and recieved into a file\
\n\r\t--encryption=KEY_FILE\toptional argument to encrypt messages using the given key. Server and client must use the same key\
\n\r\t--host=HOST_NAME\toptional argument to change the host from lnxsrv09 to another host\
\n\r\t--debug\t\t\toptional argument to print debug statements\n");  
}

//for terminal configuration
//This function is called upon system exit due to the atexit declaration 
//Thus it does not need to be called manually
void restore_original_config(){
	if (tcsetattr(STDIN_FILENO, TCSANOW, &ORIGINAL_CONFIGURATION) < 0)
		perror("Error: Failed to restore original config");
	//fix alignment
	printf("\r");
}

//This function is called to save the original terminal configuration 
//before terminal changes
void get_original_config(){
	//if STDIN_FILENO isn't a terminal, error exit. 
	if(!isatty(STDIN_FILENO)) {
		perror("Error: Problem with stdin");
		close_and_exit_program(1);
	}
	if(tcgetattr(0, &ORIGINAL_CONFIGURATION) < 0){
		perror("Error: Failed to get original config");
		close_and_exit_program(1);
	}
}

//This function is called to set a new terminal configuration
//with no echo 
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
	if(tcsetattr(STDIN_FILENO,TCSANOW, &new_config) < 0){
		perror("Error: Failed to set new config");
		close_and_exit_program(1);
	}
}

//this function checks for arguments
void process_args(int argc, char **argv){
	char* log_file, *key_file;
	static struct option long_options[] =
	{
	  	{"port", required_argument, 0, 'p'}, 
		{"log", required_argument, 0, 'l'}, 
		{"encryption", required_argument, 0, 'e'},
	    {"host", required_argument, 0, 'h'},
	    {"debug", no_argument, 0, 'd'},
	    {0, 0, 0, 0}
	};
	int c=0;
	while ((c = getopt_long(argc, argv, "p:l:e:h:d:", long_options, NULL)) != -1){
	    //no argument, break
	    if (c == -1) break;
	    switch (c)
	    {
	      case 'p': 
	      	//should be > 1024
	      	PORT_NUMBER=atoi(optarg);
	      	break;
	      case 'l': 
	      	LOG_FLAG=1;
	      	log_file=optarg;
	      	//creates log file (overwriting an older file if necessary)
			LOG_FILE_FD = open(log_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
			if(LOG_FILE_FD<0){
				perror("Error: Failed to create log file");
				close_and_exit_program(1);
			}
	      	break;
	      case 'e': 
	      	CRYPT_FLAG=1;
	      	key_file=optarg;
	      	KEY_FILE_FD = open(key_file, O_RDONLY);
			if(KEY_FILE_FD<0){
				perror("Error: Failed to open crypt file");
				close_and_exit_program(1);
			}
	      	break;
	      case 'h': 
	      	HOST_NAME=optarg;
	      	break;
	      case 'd':
	        DEBUG_FLAG=1;
	        break;
	      case '?':
	      	print_usage();
	      	close_and_exit_program(1);
	      default:
	        // should never get here. 
	      	if(DEBUG_FLAG) printf("Error: Unexpected case.\n\r");
	        close_and_exit_program(1);
	      }
  	}

	if(DEBUG_FLAG){
		printf("Debugging...\n\r");
		printf("Port set to %d\n\r", PORT_NUMBER);
		if(LOG_FLAG) printf("Logging to file %s\n\r", log_file);
		if(CRYPT_FLAG) printf("Crypting with file %s\n\r",key_file);
	}
	//catch no port number error
	if(!PORT_NUMBER){
		fprintf(stderr,"Error: Port number must be declared with argument --port=PORT_NUMBER\n\r");
		close_and_exit_program(1);
	}
}


//connect to server
void establish_connection(){
	//Referred to example from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
	//as well as http://www.binarytides.com/server-client-example-c-sockets-linux/
	struct sockaddr_in server_address;
	struct hostent* server;
	int num_bytes;
	if(DEBUG_FLAG) printf("Establishing connection...\r\n");
     
    //Create socket
    SOCKET_FD = socket(AF_INET , SOCK_STREAM , 0);
    if (SOCKET_FD == -1) {
        perror("\rError: socket creation problem"); close_and_exit_program(1);
    }

    server = gethostbyname(HOST_NAME);
	if(server == NULL) {fprintf(stderr,"\rError: failed to find host %s\r\n",HOST_NAME); close_and_exit_program(1);}
	//Initialize the server address to zero 
	memset((char*) &server_address,0, sizeof(server_address));
	//assign server_address properties
	server_address.sin_family = AF_INET;
	memcpy((char *) &server_address.sin_addr.s_addr,
			(char*) server->h_addr, 
			server->h_length);
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons( PORT_NUMBER );
    //Connect to remote server
    if (connect(SOCKET_FD , (struct sockaddr *)&server_address , sizeof(server_address)) < 0){	
    	perror("\rError: failed to connect to socket"); close_and_exit_program(1); 
    }
    if(DEBUG_FLAG) printf("\rConnected!\r\n");
}

//This read/write function is called by poll to read
//one byte at a time. It reads 256 bytes max. 
//return 0 for no bytes read or 1 for at least 1 byte read
int read_from_keyboard(){
	int buff_size=256;
	int bytes_read;
	char unmodified[buff_size+1];
	memset((char*) &unmodified,0, sizeof(unmodified));
	bytes_read=read(STDIN_FILENO, unmodified, buff_size);
	//keep reading 
	if(bytes_read<0) {
		perror("\rError: problem reading from standard input"); 
		close_and_exit_program(1);
	}
	else if(bytes_read==0) {
		if(DEBUG_FLAG) printf("\r\nRead zero bytes.\r\n");
		//other side has disconnected
		close_and_exit_program(0);
	}
	//make copy
	char encrypted[buff_size+1];
	strcpy(encrypted, (const char *)unmodified);
	//encrypt if crypt flag
	if(CRYPT_FLAG){
		encrypt(encrypted,bytes_read);
	}
	//log
	if(LOG_FLAG){
		dprintf(LOG_FILE_FD,"SENT %d bytes: %s\n",bytes_read,encrypted);
	}
	int i;
	for(i=0; i<bytes_read; i++){
    	//If carriage return or newline was entered, 
	    if (unmodified[i]=='\r' || unmodified[i]=='\n') {
	    	//rewrite as \r\n for stdout
	        write(STDOUT_FILENO, "\r\n", 2);
	        write(SOCKET_FD, encrypted+i, 1);
	    }
	    else {
	    	// non encrypted to output 
        	write(STDOUT_FILENO, unmodified+i, 1);
		    // write encrypted to server
		    write(SOCKET_FD, encrypted+i, 1);
	    }
	        
    }
}

//This read/write function is called by poll to read
//one byte at a time. It reads 256 bytes max. 
//return 0 for no bytes read or 1 for at least 1 byte read
int read_from_server(){
	int buff_size=256;
	char buffer[buff_size+1];
	memset((char*) &buffer,0, sizeof(buffer));
	int bytes_read;
	//keep reading 
	bytes_read=read(SOCKET_FD, buffer, buff_size);
	if(bytes_read<0) {
		perror("\rError: problem reading from socket"); 
		close_and_exit_program(1);
	}
	else if(bytes_read==0) {
		if(DEBUG_FLAG) printf("\r\nRead zero bytes.\r\n");
		//other side has disconnected
		close_and_exit_program(0);
	}
	
	//log
	if(LOG_FLAG){
		dprintf(LOG_FILE_FD,"RECIEVED %d bytes: %s\n",bytes_read,buffer);
	}
	//decrypt
	if(CRYPT_FLAG){
		decrypt(buffer,bytes_read);
	}
	int i;
	for(i=0; i<bytes_read; i++){
    	//If carriage return or newline was entered, rewrite as \r\n.
	    if (buffer[i]=='\r' || buffer[i]=='\n') {
	        write(STDOUT_FILENO, "\r\n", 2);
        }
	    // found nothing special, output regularly.
	    else {
	        // if reading from keyboard, write encrypted to server
	        // non encrypted to output 
	        write(STDOUT_FILENO, buffer+i, 1);
	    }
    }
}

//perform polling 
void poll_and_process_io(){
	if(DEBUG_FLAG)printf("\rPolling starting\r\n");
    struct pollfd ufds[2];
    //poll explained by http://beej.us/guide/bgnet/output/html/multipage/pollman.html
    // first poll waits for keyboard
    ufds[0].fd = STDIN_FILENO;
    // check for input or error events
    ufds[0].events = POLLIN | POLLHUP | POLLERR;
    // second poll waits for server 
    ufds[1].fd = SOCKET_FD;
    // check for input or error events
    ufds[1].events = POLLIN | POLLHUP | POLLERR;

    while(1){
	      // wait for events on the two filedescriptors, no timeout
	    int rv = poll(ufds, 2, 0);
	    if (rv == -1 ) {
	        perror("Error: poll event error");
	        close_and_exit_program(1);
	    }
	    else {
			// check for events from socket:
			while (ufds[1].revents & POLLIN) {
				//read and write shell output to output
				read_from_server();
				poll(ufds, 2, 0);
			}

		    //check for socket hangup
			if (ufds[0].revents & POLLHUP || ufds[1].revents & POLLHUP){
				if(DEBUG_FLAG) printf("Poll recieved signal POLLHUP.\r\n");
				close_and_exit_program(0);
			}

			// check for events on keyboard:
			if (ufds[0].revents & POLLIN) {
				//read and write keyboard to shell input
				read_from_keyboard();
			}

			// check for errors and hangups:
			if (ufds[0].revents & POLLERR || ufds[1].revents & POLLERR) {
				if(DEBUG_FLAG) printf("POLL recieved signal POLLERR.\r\n");
				close_and_exit_program(0);
			}
		}
  	}	
}


int main (int argc, char **argv)
{
	//to catch sigpipes
	signal(SIGPIPE,signal_handler);
	//process the arguments
	process_args(argc, argv);
	//get the original configuration of the terminal
	get_original_config();
	//change the configuration to character at a time, no echo
	set_new_config();
	//restore at exit, always
	atexit(restore_original_config);
	if(CRYPT_FLAG){
		create_key();
		init_crypt_fd();
	}
	establish_connection();
	poll_and_process_io(); 
	close_and_exit_program(0);
}