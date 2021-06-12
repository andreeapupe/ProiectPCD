#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Include for multithreading
#include <pthread.h>
#include <sys/wait.h>

// Includes for socket communication
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

// Project specific includes
#include "config/cfg.h"
#ifdef MAC

char serverResponse[256];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *socketThread(void *arg);
void write_file(int sockfd);
int createSocket();
int isAFile(char *message);

int main(int argv, char *argc[])
{
    pid_t childId;

    childId = fork();

    if (childId == -1)
    {
        perror("Cannot fork\n");
    }
    else
    {
        if (childId == 0)
        {
            // WEB COMPONENT PROCESS
            pthread_t tid[60];
            int i = 0;
            int server_socket, client_socket;

            server_socket = createSocket();

            struct sockaddr_in server_address;
            struct sockaddr_in client_address;
            struct sockaddr_storage server_storage;

            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(PORT_WEB_CLIENT);
            server_address.sin_addr.s_addr = INADDR_ANY;

            int client_address_len = sizeof(client_address);
            int bindValue;
            int listenValue;

            if ((bindValue = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1))
            {
                perror("Bind error\n");
            };

            if ((listenValue = listen(server_socket, 5)) == -1)
            {
                perror("Listen error\n");
            };

            fprintf(stdout, "Waiting for web client...\n");

            while (TRUE)
            {
                if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)
                {
                    perror("Accept failure\n");
                    exit(EXIT_FAILURE);
                };

                fprintf(stdout, "New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);

                pthread_t t;
                int *pclient = malloc(sizeof(int));
                *pclient = client_socket;
                pthread_create(&t, NULL, socketThread, pclient);
            }

            wait(NULL);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // STANDARD CLIENT PROCESS
            pthread_t tid[60];
            int i = 0;
            int server_socket, client_socket;

            server_socket = createSocket();

            struct sockaddr_in server_address;
            struct sockaddr_in client_address;
            struct sockaddr_storage server_storage;

            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(PORT_STD_CLIENT);
            server_address.sin_addr.s_addr = INADDR_ANY;

            int client_address_len = sizeof(client_address);

            int bindValue;
            int listenValue;

            if ((bindValue = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1))
            {
                perror("Bind error\n");
            };

            if ((listenValue = listen(server_socket, 5)) == -1)
            {
                perror("Listen error\n");
            };

            fprintf(stdout, "Waiting for connections...\n");

            while (TRUE)
            {
                if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)
                {
                    perror("Accept failure\n");
                    exit(EXIT_FAILURE);
                };

                fprintf(stdout, "New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);

                pthread_t t;
                int *pclient = malloc(sizeof(int));
                *pclient = client_socket;
                pthread_create(&t, NULL, socketThread, pclient);
            }

            wait(NULL);
            exit(EXIT_SUCCESS);
        }
    }
}

int createSocket()
{
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    };

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
    {
        perror("Cannot assign SO_REUSEADDR");
        exit(EXIT_FAILURE);
    };

    return server_socket;
}

void *socketThread(void *arg)
{
    int newSocket = *((int *)arg);
    char clientMessage[1024];

    recv(newSocket, &clientMessage, 1024, 0);
    if (strcmp(clientMessage, "FILE_INCOMING") == 0)
    {
        char *ciuciu = "Mesaj ciuciu";
        send(newSocket, ciuciu, sizeof(ciuciu), 0);
        printf("TEST\n\n");
        write_file(newSocket);
    }
    else
    {
        //printf("From client: %s\n\n", clientMessage);
        printf("matan cur");
    }

    pthread_mutex_lock(&lock);
    strcpy(serverResponse, "HTTP/1.1 200 OK\r\n\r\n");
    //sleep(5);
    pthread_mutex_unlock(&lock);

    send(newSocket, serverResponse, sizeof(serverResponse), 0);
    printf("[-]Exit socket thread \n");
    close(newSocket);
    pthread_exit(NULL);
}

int isAFile(char *message)
{
    // dummy
    return 0;
}

void write_file(int sockfd)
{
    int n;
    FILE *fp;
    char *filename = "file2.mp4"; // filename should be generated
    char buffer[FILE_SIZE_CHUNK];

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror("error at file creation");
        return;
    }

    while (1)
    {
        n = recv(sockfd, buffer, FILE_SIZE_CHUNK, 0);
        printf("Received: %d bytes\n", n);
        if (n <= 0)
        {
            break;
            return;
        }
        fwrite(buffer, n, 1, fp);
        bzero(buffer, FILE_SIZE_CHUNK);
    }
    return;
}
#endif
