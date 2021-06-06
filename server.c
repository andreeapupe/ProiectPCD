#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Include for multithreading
#include <pthread.h>
#include <wait.h>

// Includes for socket communication
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>
#include <arpa/inet.h>

// Project specific includes
#include "config/cfg.h"

char serverResponse[256];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void* socketThread(void* arg)
{
    int newSocket = *((int *)arg);
    char clientMessage[1024];

    recv(newSocket, &clientMessage, 1024, 0);
    printf("From client: %s\n\n", clientMessage);

    pthread_mutex_lock(&lock);
    strcpy(serverResponse, "HTTP/1.1 200 OK\r\n\r\n");
    //sleep(5);
    pthread_mutex_unlock(&lock);
    
    send(newSocket, serverResponse, sizeof(serverResponse), 0);
    printf("[-]Exit socket thread \n");
    close(newSocket);
    pthread_exit(NULL);
}

int main(int argv, char* argc[])
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
            // Child process
            pthread_t tid[60];
            int i = 0;
            int server_socket, client_socket;

            server_socket = socket(AF_INET, SOCK_STREAM, 0);

            struct sockaddr_in server_address;
            struct sockaddr_in client_address;
            struct sockaddr_storage server_storage;

            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(PORT_WEB_CLIENT);
            server_address.sin_addr.s_addr = INADDR_ANY;

            int client_address_len = sizeof(client_address);
            int bindValue;
            int listenValue;
            
            if ((bindValue = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1))
            {
                perror("Bind error\n");
            };

            if ((listenValue = listen(server_socket, 5)) == -1)
            {
                perror("Listen error\n");
            };

            printf("Waiting for connections...\n");

            while(TRUE)
            {
                if((client_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&client_address_len)) < 0)
                {
                    perror("Accept failure\n");
                    exit(EXIT_FAILURE);
                };
                
                printf("New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);

                pthread_t t;
                int* pclient = malloc(sizeof(int));
                *pclient = client_socket;
                pthread_create(&t, NULL, socketThread, pclient);
            }

            return 0;
            wait(NULL);
        }
        else
        {
            // Parent process
            pthread_t tid[60];
            int i = 0;
            int server_socket, client_socket;

            server_socket = socket(AF_INET, SOCK_STREAM, 0);

            struct sockaddr_in server_address;
            struct sockaddr_in client_address;
            struct sockaddr_storage server_storage;

            server_address.sin_family = AF_INET;
            server_address.sin_port = htons(PORT_STD_CLIENT);
            server_address.sin_addr.s_addr = INADDR_ANY;

            int client_address_len = sizeof(client_address);

            bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

            listen(server_socket, 5);

            printf("Waiting for connections...\n");

            while(TRUE)
            {
                if((client_socket = accept(server_socket, (struct sockaddr*)&client_address, (socklen_t *)&client_address_len)) < 0)
                {
                    perror("Accept failure\n");
                    exit(EXIT_FAILURE);
                };
                
                printf("New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);

                pthread_t t;
                int* pclient = malloc(sizeof(int));
                *pclient = client_socket;
                pthread_create(&t, NULL, socketThread, pclient);
            }

            return 0;
            wait(NULL);
        }
    }

    

}