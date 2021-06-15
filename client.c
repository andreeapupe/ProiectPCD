#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "config/cfg.h"
#include "car.h"

// MAX = Maximum number of octets
#define MAX 1024
#define SA struct sockaddr
;

#define EXEC_MODE_UNKNOWN 0
#define EXEC_MODE_SEND 1
#define EXEC_MODE_GET 2

int validate_args(int argc, char *argv[], int *execMode, char *path);
void handle_send_connection(char *path, int sockfd);

int main(int argc, char *argv[])
{
	int sockfd, connfd;
	int execMode;
	char xmlPath[256];
	struct sockaddr_in servaddr, cli;

	if (FALSE == validate_args(argc, argv, &execMode, xmlPath)) {
		return 0;
	}

	printf("Exec mode: %d\n", execMode);
	printf("XML Path: %s\n", xmlPath);

	// Socket creation
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("[-] Socket creation failed...\n");
		exit(0);
	}
	else
	{
		printf("[+] Socket successfully created...\n");
	}

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT_STD_CLIENT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("[-] Client cannot connect to server...\n");
	}
	else
	{
		printf("[+] Successfully connected to the server..\n\n");
	}

	if (EXEC_MODE_SEND == execMode) {
		Car_t car;
		if (TRUE == parse_car(xmlPath, &car)) {
			handle_send_connection(car.attachmentPath, sockfd);
		}
	} 
	else{
		//TODO: handle get_connection
	}
	close(sockfd);

	return 0;
}

int validate_args(int argc, char *argv[], int *execMode, char *filename)
{
	*execMode = EXEC_MODE_UNKNOWN;
	filename[0] = 0;
	if (3 == argc) {
		for (int i = 1; i < argc; i++) {
			 char *arg = argv[i];
			 if (0 == strcmp("--send", arg)) {
			 	 *execMode = EXEC_MODE_SEND;
			 }
			 else
			 if (0 == strcmp("--get", arg)) {
			 	 *execMode = EXEC_MODE_GET;
			 }
			 else {
			 	 strcpy(filename, arg);
			 }
		}
	}

	if (execMode == EXEC_MODE_UNKNOWN || 0 == strlen(filename)) {
		printf("Invalid arguments\n./client [--send|--get] <path_to_xml_file>");
		return FALSE;
	}
	return TRUE;
}

void sendFile(char *path, int sockfd);
void handle_send_connection(char *path, int sockfd)
{
	char buff[MAX];
	int n;

	bzero(buff, sizeof(buff));
	strcpy(buff, "FILE_INCOMING");
	write(sockfd, buff, sizeof(buff));
	sendFile(path, sockfd);

	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff));
	printf("Server: %s", buff);
}

void sendFile(char *path, int sockfd)
{
#if MAC
	unsigned char buffer[1024];
	size_t bytesRead = 0;
	char buff[1024] = "FILE_INCOMING";

	FILE *fd1 = fopen(path, "rb");

	if (NULL == fd1) {
		printf("Cannot open file at path: %s", path);
		return;
	}

	while ((bytesRead = fread(buffer, 1, sizeof(buffer), fd1)) > 0)
	{
		send(sockfd, buffer, sizeof(buffer), 0);
	}
	close(sockfd);

	fclose(fd1);

#else
	int fd;
	struct stat stbuf;

	fd = open(path, O_RDONLY); // should be get from xml

	if (0 == fd) {
		printf("Cannot open file at path: %s", path);
		return;
	}
	fstat(fd, &stbuf);

	sendfile(sockfd, fd, 0, stbuf.st_size);
	close(sockfd);
	close(fd);
#endif
}
