//allow interaction with server

extern int SOCKET_FD;
char* CONNECT_TYPE; //either TCP or TLS. Defined in connectTCP and connectTLS

//reads from server and returns bytes read
int readFromServer(char* buffer, int buff_size);

//writes to server
void writeToServer(char* my_string, int write_amount);

//connects to either TLS or TCP server
void connectMe(int PORT, char* HOST);
