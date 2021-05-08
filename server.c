#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
// MAX = Maximum number of octets
#define MAX 1024 
// Used port
#define PORT 5000
#define SA struct sockaddr

// For chat between client and server
void func(int sockfd)
{
	char buff[MAX];
	int n;


	// Creating an infinite loop for chat between client and server
	for (;;) {
		bzero(buff, MAX);

		// Read message from client and copy it in buffer
		read(sockfd, buff, sizeof(buff));

		// Print buffer after copying
		printf("\tFROM client: %sTO client: ", buff);
		bzero(buff, MAX);
		n = 0;

		// Copy server message in buffer
		while ((buff[n++] = getchar()) != '\n')
			;

		// Send buffer to client
		write(sockfd, buff, sizeof(buff));

		// If one of the messages is "goodbye", server ends the conversation
		if (strncmp("goodbye", buff, 7) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}


int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// Socket creation
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// Assigning IP and a PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding socket to IP
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Check if server is ready to listen
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("Server acccept failed...\n");
		exit(0);
	}
	else
		printf("Server acccepted the client...\n\n");
	
	func(connfd);
	close(sockfd);
}

