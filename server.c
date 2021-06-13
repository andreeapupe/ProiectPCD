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

char serverResponse[256];
char logs[1024];

pthread_mutex_t lock      = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_logs = PTHREAD_MUTEX_INITIALIZER;

void *socketThread(void *arg);
void *socketThreadAdministrator(void *arg);
void *adminComponentThread(void *arg);
void start_admin_component(int port, int *server_sock, int *client_sock);
void write_file(int sockfd);
void init_admin_component(pthread_t *threadId, int port);
int createSocket();
int isAFile(char *message);

int main(int argv, char *argc[])
{
    pid_t childId;

    childId = fork();

    if (childId == -1)
    {
        perror("[-] Cannot fork\n");
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
                perror("[-] Bind error WEB CLIENT on port 8080");
                
                fprintf(stdout, "[+] Trying bind socket on port %d\n", PORT_WEB_CLIENT_ALTERNATIVE);
                server_address.sin_port = htons(PORT_WEB_CLIENT_ALTERNATIVE);
                if ((bindValue = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1))
                {
                    exit(EXIT_FAILURE);    
                } 

                fprintf(stdout, "[+] Sucessfully binding on port %d\n", PORT_WEB_CLIENT_ALTERNATIVE);
            };

            if ((listenValue = listen(server_socket, 5)) == -1)
            {
                perror("[-] Listen error WEB CLIENT");
                exit(EXIT_FAILURE);
            };

            fprintf(stdout, "[+] Waiting for web clients...\n");

            while (TRUE)
            {
                if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)
                {
                    perror("[-] Accept failure");
                    exit(EXIT_FAILURE);
                };

                fprintf(stdout, "[+] New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);
                

                char buffer_port[12];
                pthread_mutex_lock(&lock_logs);
                strcat(logs, "[+] New connection from ");
                strcat(logs, inet_ntoa(client_address.sin_addr));
                //_itoa((int)client_address.sin_port, buffer_port, 10);
                sprintf(buffer_port, "%d", (int)client_address.sin_port);
                strcat(logs, ":");
                strcat(logs, buffer_port);
                pthread_mutex_unlock(&lock_logs);


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
            pthread_t admin_component;

            init_admin_component(&admin_component, PORT_ADM_CLIENT);
            bzero(&logs, sizeof(logs));

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
                perror("[-] Bind error STD CLIENT");
                exit(EXIT_FAILURE);
            };

            if ((listenValue = listen(server_socket, 5)) == -1)
            {
                perror("[-] Listen error STD CLIENT");
                exit(EXIT_FAILURE);
            };

            fprintf(stdout, "[+] Waiting for standard clients...\n");

            while (TRUE)
            {
                if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)
                {
                    perror("[-] Accept failure");
                    exit(EXIT_FAILURE);
                };

                fprintf(stdout, "[+] New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);
                
                char buffer_port[12];
                pthread_mutex_lock(&lock_logs);
                strcat(logs, "[+] New connection from ");
                strcat(logs, inet_ntoa(client_address.sin_addr));
                //_itoa((int)client_address.sin_port, buffer_port, 10);
                sprintf(buffer_port, "%d", (int)client_address.sin_port);
                strcat(logs, ":");
                strcat(logs, buffer_port);
                pthread_mutex_unlock(&lock_logs);

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
        perror("[-] Cannot create socket");
        exit(EXIT_FAILURE);
    };

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) == -1)
    {
        perror("[-] Cannot assign SO_REUSEADDR");
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
        write_file(newSocket);
    }
    else
    {
        //printf("From client: %s\n\n", clientMessage);
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

void *socketThreadAdministrator(void *arg)
{
    int client_socket = *((int *)arg);
    char clientMessage[1024];
    char sampleMessage[1024] = "From admin";


    recv(client_socket, &clientMessage, 1024, 0);
    printf("Admin: %s\n", clientMessage);

    while(1)
    {
        pthread_mutex_lock(&lock_logs);
        if(strlen(logs) != 0)
        {
            send(client_socket, logs, sizeof(logs), 0);
            strcpy(logs, "");

        }
        pthread_mutex_unlock(&lock_logs);
        
    }

    printf("[-]Exit socket thread \n");

    close(client_socket);
    arg = 0x00000000;
}

void *adminComponentThread(void *arg)
{
    //printf("Admin component started\n");
    char clientMessage[1024];
    int port = *((int*)arg);
    int server_socket, client_socket;

    server_socket = createSocket();
    bzero(&client_socket, sizeof(client_socket));

    struct sockaddr_in server_address;
    struct sockaddr_in client_address;
    struct sockaddr_storage server_storage;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int client_address_len = sizeof(client_address);

    int bindValue;
    int listenValue;

    if ((bindValue = bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1))
    {
        perror("[-] Bind error");
        exit(EXIT_FAILURE);
    };

    if ((listenValue = listen(server_socket, 5)) == -1)
    {
        perror("[-] Listen error");
        exit(EXIT_FAILURE);
    };

    fprintf(stdout, "[+] Waiting for administrator...\n");
    
    while (TRUE)
    {
        // Ensure only one administrator is connected
        if (client_socket == 0x00000000)
        {
            if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)  
            {
                perror("[-] Accept failure");
                exit(EXIT_FAILURE);
            };

            fprintf(stdout, "[+] New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);

            pthread_t t;
            int *pclient = malloc(sizeof(int));
            *pclient = client_socket;
            pthread_create(&t, NULL, socketThreadAdministrator, pclient);
        }
        else
        {
            char* serverResponse = "[-] Only one administrator is allowed!";
            if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)  
            {
                perror("[-] Accept failure");
                exit(EXIT_FAILURE);
            };

            fprintf(stdout, "[-] Refused connection connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);

            recv(client_socket, &clientMessage, 1024, 0);
            //fprintf(stdout, "A new admin component said: %s\n", clientMessage);
            send(client_socket, serverResponse, sizeof(serverResponse), 0);
            close(client_socket);
        }
    }
    
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
        perror("[-] error at file creation");
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
    fclose(fp);
    return;
}

void init_admin_component(pthread_t *threadId, int port)
{
    int *pPort = malloc(sizeof(int));
    *pPort = port;

    if (pthread_create(threadId, NULL, adminComponentThread, pPort) == -1)
    {
        perror("[-] Cannot create admin component thread");
        exit(EXIT_FAILURE);
    };

    //printf("Administrator component thread successfully initialized...\n");
}