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

// Includes for interprocess communications
#include  <sys/ipc.h>
#include  <sys/shm.h>

// Project specific includes
#include "config/cfg.h"
#include "web.h"

char serverResponse[256];
char logs[1024];
char errorLogs[2048];
char clientMessageAdministrator[1024];
char source[4096];
char* pSharedMemoryLogs;

int countFiles = 0;

pthread_mutex_t lock            = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_logs       = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_error_logs = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_shm_logs   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_source     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_client_msg   = PTHREAD_MUTEX_INITIALIZER;


void *socketThreadStandard(void *arg);
void *socketThreadAdministrator(void *arg);
void* socketThreadWeb(void* arg);
void *adminComponentThread(void *arg);
void *handlerRemoveProcess(void *arg);
void start_admin_component(int port, int *server_sock, int *client_sock);
void write_file(int sockfd);
void init_admin_component(pthread_t *threadId, char *shmValue);
void concatErrorLogs(const char* message);
int createSocket();
int isAFile(char *message);

int main(int argv, char *argc[])
{
    pid_t childId;
    int sharedMemoryLogsId = shmget(IPC_PRIVATE, 1024*sizeof(char), IPC_CREAT | 0666);
    pSharedMemoryLogs = (char*)shmat(sharedMemoryLogsId, NULL, 0);

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
            pthread_t tid[MAX_CLIENTS_NUMBER];
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
                pthread_mutex_lock(&lock_shm_logs);
                strcat(pSharedMemoryLogs, "[+] New web connection ");
                strcat(pSharedMemoryLogs, inet_ntoa(client_address.sin_addr));
                //_itoa((int)client_address.sin_port, buffer_port, 10);
                sprintf(buffer_port, "%d", (int)client_address.sin_port);
                strcat(pSharedMemoryLogs, ":");
                strcat(pSharedMemoryLogs, buffer_port);
                strcat(pSharedMemoryLogs, "\n");
                pthread_mutex_unlock(&lock_shm_logs);
                
                int *pclient = malloc(sizeof(int));
                *pclient = client_socket;
                pthread_create(&tid[i], NULL, socketThreadWeb, pclient);
                i++;
            }

            wait(NULL);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // STANDARD CLIENT PROCESS
            pthread_t tid[MAX_CLIENTS_NUMBER];
            pthread_t admin_component;

            init_admin_component(&admin_component, pSharedMemoryLogs);
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
                perror(BIND_STD_ERROR_MESSAGE);
		            concatErrorLogs(BIND_STD_ERROR_MESSAGE);
                exit(EXIT_FAILURE);
            };

            if ((listenValue = listen(server_socket, 5)) == -1)
            {
                perror(LISTEN_STD_ERROR_MESSAGE);
		            concatErrorLogs(LISTEN_STD_ERROR_MESSAGE);
                exit(EXIT_FAILURE);
            };

            fprintf(stdout, "[+] Waiting for standard clients...\n");

            while (TRUE)
            {
                if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)
                {
                    perror(ACCEPT_ERROR_MESSAGE);
		                concatErrorLogs(ACCEPT_ERROR_MESSAGE);
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
                strcat(logs, "\n");
                pthread_mutex_unlock(&lock_logs);

                int *pclient = malloc(sizeof(int));
                *pclient = client_socket;
                pthread_create(&tid[i], NULL, socketThreadStandard, pclient);
                i++;
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

void *handlerRemoveProcess(void *arg)
{
    int client_socket = *((int *)arg);

    while(TRUE)
    {
        pthread_mutex_lock(&lock_client_msg);
        recv(client_socket, &clientMessageAdministrator, 1024, 0);
        printf("Admin: %s\n", clientMessageAdministrator);
        pthread_mutex_unlock(&lock_client_msg);

        if (strstr(clientMessageAdministrator, "REMOVE_LAST") != 0)
        {
            
        }

        pthread_mutex_lock(&lock_client_msg);
        strcpy(clientMessageAdministrator, "");
        pthread_mutex_unlock(&lock_client_msg);

        sleep(1);
    }
}

void *socketThreadStandard(void *arg)
{
    int newSocket = *((int *)arg);
    char clientMessage[1024];

    recv(newSocket, &clientMessage, 1024, 0);

    if (strcmp(clientMessage, "FILE_INCOMING") == 0)
    {
        fprintf(stdout, "%s\n", clientMessage);
        write_file(newSocket);
    }
    else
    {
        if(strstr(clientMessage, "INFO_INCOMING") != NULL)
        {
           strcpy(source, "clientMessage");
        }
    }

    // // post/get request from web
    if(checkHttpReqType(clientMessage)==1 ||(checkHttpReqType(clientMessage)==2))
    {
        char response[2048];
        strcpy(response, responseCode(clientMessage,"403"));
        send(newSocket, response, sizeof(response), 0);
        //close(newSocket);
        //pthread_exit(NULL);
    }
    else
    {
        //msg request from standard client
        strcpy(serverResponse,clientMessage);
    }

    //fprintf(stdout, "%s",serverResponse);
    send(newSocket, serverResponse, sizeof(serverResponse), 0);
    printf("[-]Exit socket thread\n");
    close(newSocket);
    pthread_exit(NULL);
}

void *socketThreadAdministrator(void *arg)
{
    struct arg_struct *args = arg;
    pthread_t listenForRemoveSignals;

    int client_socket = *args->ptr;
    char pSharedMemoryLogsLocal[1024];

    strcpy(pSharedMemoryLogsLocal, args->shm);

    char sampleMessage[1024] = "From admin";

    pthread_create(&listenForRemoveSignals, NULL, handlerRemoveProcess, (void *)&client_socket);

    while(1)
    {
        pthread_mutex_lock(&lock_source);
        if(strlen(source) != 0)
        {
            send(client_socket, source, sizeof(source), 0);
            strcpy(source, "");
        }
        pthread_mutex_unlock(&lock_source);


        pthread_mutex_lock(&lock_logs);
        if(strlen(logs) != 0 || strlen(pSharedMemoryLogs) != 0)
        {
            pthread_mutex_lock(&lock_shm_logs);
            strcat(logs, pSharedMemoryLogs);
            pthread_mutex_unlock(&lock_shm_logs);
            send(client_socket, logs, sizeof(logs), 0);
            strcpy(logs, "");
            strcpy(pSharedMemoryLogs, "");
            strcpy(pSharedMemoryLogsLocal, "");
        }
        pthread_mutex_unlock(&lock_logs);
        
    }

    printf("[-]Exit socket thread \n");

    close(client_socket);
    arg = 0x00000000;
}

void* socketThreadWeb(void* arg)
{
    int newSocket = *((int *)arg);
    char clientMessage[1024];

    recv(newSocket, &clientMessage, 1024, 0);
    printf("From client: %s\n\n", clientMessage);

    pthread_mutex_lock(&lock);

    // post/get request from web
    if(checkHttpReqType(clientMessage)==1 ||(checkHttpReqType(clientMessage)==2))
    {
        strcpy(serverResponse,responseCode(defaultHttpResponse,"200"));

        struct returnExample structura = reqToStruct(clientMessage);
        if(strcmp(structura.bdy[0].value,"send")==0)
            for (int i =0;i< atoi(structura.bdy[1].value);i++)
            {
                system(xmlSendList[i]);
            }
        if(strcmp(structura.bdy[0].value,"get")==0)
            for (int i =0;i< atoi(structura.bdy[1].value);i++)
            {
                system(xmlGetList[i]);
            }
         

    }
    //sleep(5);
    pthread_mutex_unlock(&lock);

   // printf("%s",serverResponse);
    send(newSocket, serverResponse, sizeof(serverResponse), 0);
    //bzero(&serverResponse, sizeof(serverResponse));
    printf("[-]Exit socket thread \n");
    close(newSocket);
    pthread_exit(NULL);
}

void *adminComponentThread(void *arg)
{
    //printf("Admin component started\n");
    char clientMessage[1024];

    int port = PORT_ADM_CLIENT;

    char* shmValue = (char*)arg;
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
        perror(BIND_ADMIN_ERROR_MESSAGE);
	      concatErrorLogs(BIND_ADMIN_ERROR_MESSAGE);
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
                perror(ACCEPT_ERROR_MESSAGE);
		            concatErrorLogs(ACCEPT_ERROR_MESSAGE);
                exit(EXIT_FAILURE);
            };

            fprintf(stdout, "[+] New connection from %s:%d\n", inet_ntoa(client_address.sin_addr), (int)client_address.sin_port);

            struct arg_struct args;

            pthread_t t;
            int *pclient = malloc(sizeof(int));
            *pclient = client_socket;

            args.ptr = pclient;
            strcpy(args.shm, shmValue);
            


            pthread_create(&t, NULL, socketThreadAdministrator, (void *)&args);
        }
        else
        {
            char* serverResponse = "[-] Only one administrator is allowed!";
            if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len)) < 0)  
            {
                perror(ACCEPT_ERROR_MESSAGE);
		            concatErrorLogs(ACCEPT_ERROR_MESSAGE);
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
    long long int countBytes = 0;
    int n;
    FILE *fp;
    char filename[64];
    char buffer[FILE_SIZE_CHUNK];
    char bufferCounter[12];
    
    sprintf(bufferCounter, "%d", countFiles);

    strcat(filename, "video_");
    strcat(filename, bufferCounter);
    strcat(filename, ".mp4");

    countFiles++;
    strcpy(bufferCounter, "");

    fp = fopen(filename, "wb");
    if (fp == NULL)
    {
        perror(FILE_CREATION_ERROR_MESSAGE);
	    concatErrorLogs(FILE_CREATION_ERROR_MESSAGE);
        return;
    }
    
    while (1)
    {
        n = recv(sockfd, buffer, FILE_SIZE_CHUNK, 0);
        countBytes += n;
        if (n <= 0)
        {
            break;
            return;
        }
        fwrite(buffer, n, 1, fp);
        bzero(buffer, FILE_SIZE_CHUNK);
    }
    fprintf(stdout, "Received %lld bytes\n", countBytes);
    fclose(fp);

    system("python3 ../python/DetectCars.py ../build/min2_1.mp4");

    return;
}

void init_admin_component(pthread_t *threadId, char* shmValue)
{
    //int *pPort = malloc(sizeof(int));
    //*pPort = port;

    if (pthread_create(threadId, NULL, adminComponentThread, shmValue) == -1)
    {
        perror(ADMIN_THREAD_ERROR_MESSAGE);
	      concatErrorLogs(ADMIN_THREAD_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    };

    //printf("Administrator component thread successfully initialized...\n");
}

void concatErrorLogs(const char* message)
{
    strcat(errorLogs, message);
    strcat(errorLogs, "\n");
}