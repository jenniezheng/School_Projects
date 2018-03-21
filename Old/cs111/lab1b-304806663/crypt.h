#ifndef CRYPT_INCLUDED
#define CRYPT_INCLUDED
#include <mcrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

//external variables
int CRYPT_FLAG, KEY_FILE_FD;
char *CRYPT_FILE;
MCRYPT ENCRYPT_FD, DECRYPT_FD;
int KEY_LENGTH,FAILURE_EXIT; 
char* KEY;

//obtain the key
void create_key()
{
	struct stat file_stat;
	if(fstat(KEY_FILE_FD, &file_stat) < 0) { perror("\rError: failed to find fstat of keyfile"); exit(FAILURE_EXIT); }
	KEY_LENGTH = file_stat.st_size;
	KEY = (char*) malloc(file_stat.st_size * sizeof(char));
	if(read(KEY_FILE_FD, KEY, file_stat.st_size) < 0) { perror("\rError: failed to read key"); exit(FAILURE_EXIT); }
}

//opens the encryption and decryption modules using blowfish and ofb
void init_crypt_fd()
{
	ENCRYPT_FD = mcrypt_module_open("blowfish", NULL, "ofb", NULL);
	if(ENCRYPT_FD == MCRYPT_FAILED) { perror("\rError: failed to open module mcrypt"); exit(EXIT_FAILURE); }
	if(mcrypt_generic_init(ENCRYPT_FD, KEY, KEY_LENGTH, NULL) < 0) { perror("\rError: failed to init mcrypt"); exit(FAILURE_EXIT); }

	DECRYPT_FD = mcrypt_module_open("blowfish", NULL, "ofb", NULL);
	if(DECRYPT_FD == MCRYPT_FAILED) { perror("\rError: failed to open module mcrypt"); exit(EXIT_FAILURE); }
	if(mcrypt_generic_init(DECRYPT_FD, KEY, KEY_LENGTH, NULL) < 0) { perror("\rError: failed to init mcrypt"); exit(FAILURE_EXIT); }
}

//closes the modules that were opened for encryption/decryption
void deinit_crypt_fd()
{
	mcrypt_generic_deinit(ENCRYPT_FD);
	mcrypt_module_close(ENCRYPT_FD);
	mcrypt_generic_deinit(DECRYPT_FD);
	mcrypt_module_close(DECRYPT_FD);
}

void encrypt(char *message, int cryption_length)
{
	mcrypt_generic(ENCRYPT_FD, message, cryption_length);
}

void decrypt(char *message, int decryption_length)
{
	mdecrypt_generic(DECRYPT_FD, message, decryption_length);
}

#endif