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

// MAX = Maximum number of octets
#define MAX 1024
#define SA struct sockaddr

void validate_args(int argc, char *argv[], int execMode, char *filename);
void handle_connection(int sockfd);
void sendFile();

int main(int argc, char *argv[])
{
	int sockfd, connfd;
	int execMode;
	char filename[256];
	struct sockaddr_in servaddr, cli;

	validate_args(argc, argv, execMode, filename);

	printf("Exec mode: %d\n", execMode);
	printf("Filename: %s\n", filename);

	// Socket creation
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
	{
		printf("Socket successfully created...\n");
	}

	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT_STD_CLIENT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
	{
		printf("Client cannot connect to server...\n");
		exit(0);
	}
	else
	{
		printf("Successfully connected to the server..\n\n");
	}

	handle_connection(sockfd);
	close(sockfd);
}

void validate_args(int argc, char *argv[], int execMode, char *filename)
{
	int c;

	while (1)
	{
		int option_index = 0;
		static struct option long_options[] =
			{
				{"send", required_argument, 0, 0},
				{"get", required_argument, 0, 0},
				{0, 0, 0, 0}};

		if ((c = getopt_long(argc, argv, "s:g:", long_options, &option_index) == -1))
		{
			break;
		}

		switch (c)
		{
		case 0:
			printf("option %s", long_options[option_index].name);

			if (option_index == 0)
			{
				execMode = 1;
			}

			if (option_index == 1)
			{
				execMode = 2;
			}

			if (optarg)
			{
				strcpy(filename, optarg);
				printf(" with arg %s\n", optarg);
			}
			break;
		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}

	if (optind < argc)
	{
		printf("non-option arguments: ");
		while (optind < argc)
		{
			printf("%s\n", argv[optind++]);
		}
	}
}

void handle_connection(int sockfd)
{
	char buff[MAX];
	int n;

	bzero(buff, sizeof(buff));
	strcpy(buff, "FILE_INCOMING");
	write(sockfd, buff, sizeof(buff));
	sendFile(sockfd);

	bzero(buff, sizeof(buff));
	read(sockfd, buff, sizeof(buff));
	printf("\tServer: %s", buff);
}

void sendFile(int sockfd)
{
#if MAC
	unsigned char buffer[1024];
	size_t bytesRead = 0;
	char buff[1024] = "FILE_INCOMING";

	FILE *fd1 = fopen("min2_1.mp4", "rb");

	send(sockfd, buff, sizeof(buff), 0);
	while ((bytesRead = fread(buffer, 1, sizeof(buffer), fd1)) > 0)
	{
		send(sockfd, buffer, sizeof(buffer), 0);
	}
	close(sockfd);

	fclose(fd1);

#else
	int fd;
	struct stat stbuf;

	fd = open("min2_1.mp4", O_RDONLY); // should be get from command line
	fstat(fd, &stbuf);

	sendfile(sockfd, fd, 0, stbuf.st_size, 0, 0);
	close(sockfd);
	close(fd);
#endif
}
