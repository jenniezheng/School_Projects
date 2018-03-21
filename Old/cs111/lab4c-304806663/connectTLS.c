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

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

extern int SOCKET_FD;
#define CONNECT_TYPE "TLS";
SSL* SSL_SERVER;

int readFromServer(char* buffer, int buff_size){
	return SSL_read(SSL_SERVER, buffer,buff_size-1);
}

void writeToServer(char* my_string, int write_amount){
	SSL_write(SSL_SERVER,my_string, write_amount);
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

	SSL_library_init();

	SSL_CTX* ssl_ctx = SSL_CTX_new(SSLv23_client_method());
	SSL_SERVER = SSL_new(ssl_ctx);
	SSL_set_fd(SSL_SERVER, SOCKET_FD);
	if(SSL_connect(SSL_SERVER) != 1){
		fprintf(stderr,"Error: Failed to connect to TLS server\n");
		exit(2);
	}
}