#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "config/cfg.h"

int main(int argv, char* argc[])
{
    int server_socket;

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror(CREATE_SOCKET_ERROR_MESSAGE);
	    //concatErrorLogs(CREATE_SOCKET_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    };

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
    {
        perror(ASSIGN_SO_REUSEADDR_ERROR_MESSAGE);
	    //concatErrorLogs(ASSIGN_SO_REUSEADDR_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    };

    struct sockaddr_in server_address;
    
    bzero(&server_address, sizeof(server_address));
    
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_ADM_CLIENT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0)
	{
		printf("[-] Client cannot connect to server...\n");
		exit(0);
	}
    
    fprintf(stdout, "[+] Connected to server\n");
    char buff[1024];

	bzero(buff, sizeof(buff));
	strcpy(buff, "ADMIN HERE");
	write(server_socket, buff, sizeof(buff));

    bzero(buff, sizeof(buff));

    while(1)
    {
        read(server_socket, buff, sizeof(buff));
        printf("Server: %s\n", buff);   
    }
    
    close(server_socket);

    exit(EXIT_SUCCESS);
}