//NAME: Jennie Zheng
//EMAIL: jenniezheng321@gmail.com
//ID: 304806663


#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


char* CONNECT_TYPE="TCP";
extern int SOCKET_FD;

int readFromServer(char* buffer, int buff_size){
	return read(SOCKET_FD, buffer,buff_size-1);
}

void writeToServer(char* my_string, int write_amount){
	write(SOCKET_FD, my_string, write_amount);
}

void connectMe(int PORT_NUMBER, char* HOST_NAME){

	//Referred to example from http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html
	//as well as http://www.binarytides.com/server-client-example-c-sockets-linux/
	struct sockaddr_in server_address;
	struct hostent* server;
     
    //Create socket
    SOCKET_FD = socket(AF_INET , SOCK_STREAM , 0);
    if (SOCKET_FD == -1) {
        fprintf(stderr,"Error: Failed to open socket\n");
		exit(2);
    }

    server = gethostbyname(HOST_NAME);
	if(server == NULL) {
		fprintf(stderr,"\rError: failed to find host %s\r\n",HOST_NAME); 
		exit(2);
	}
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
    	fprintf(stderr,"Error: Failed to connect\n");
		exit(2);
    }
}

