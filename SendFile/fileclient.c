#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>

#define PORT 31337

int parseARGS(char **args, char *line){
int tmp=0;
args[tmp] = strtok( line, ":" );
while ( (args[++tmp] = strtok(NULL, ":" ) ) != NULL );
return tmp - 1;
}

int main()
{
    char *header[4096];
    int listenSOCKET, connectSOCKET;
    socklen_t clientADDRESSLENGTH;
    struct sockaddr_in clientADDRESS, serverADDRESS;
    char recvBUFF[4096];
    char *filename, *filesize;
    FILE * recvFILE;
    int received = 0;
    char tempstr[4096];

    int count1=1,count2=1, percent;

    listenSOCKET = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSOCKET < 0) {
        printf("Cannot create socket\n");
        close(listenSOCKET);
        return 1;
    }

    serverADDRESS.sin_family = AF_INET;
    serverADDRESS.sin_addr.s_addr = htonl(INADDR_ANY);
    serverADDRESS.sin_port = htons(PORT);
  
    if (bind(listenSOCKET, (struct sockaddr *) &serverADDRESS, sizeof(serverADDRESS)) < 0) {
        printf("Cannot bind socket\n");
        close(listenSOCKET);
        return 1;
    }

    listen(listenSOCKET, 5);
    clientADDRESSLENGTH = sizeof(clientADDRESS);
    connectSOCKET = accept(listenSOCKET, (struct sockaddr *) &clientADDRESS, &clientADDRESSLENGTH);
    if (connectSOCKET < 0) {
        printf("Cannot accept connection\n");
        close(listenSOCKET);
        return 1;
    }
    while(1){    
        if( recv(connectSOCKET, recvBUFF, sizeof(recvBUFF), 0) ){
            if(!strncmp(recvBUFF,"FBEGIN",6)) {
                recvBUFF[strlen(recvBUFF) - 2] = 0;
                parseARGS(header, recvBUFF);
                filename = header[1];
                filesize = header[2];
            }
            recvBUFF[0] = 0;
            recvFILE = fopen ( filename,"w" );
            percent = atoi( filesize ) / 100;
            while(1){
                if( recv(connectSOCKET, recvBUFF, 1, 0) != 0 ) {
                    fwrite (recvBUFF , sizeof(recvBUFF[0]) , 1 , recvFILE );

                    if( count1 == count2 ) {
                        printf("33[0;0H"); //move cursor to 0, 0
                        printf( "\33[2J"); //clear line
                        printf("Filename: %s\n", filename);
                        printf("Filesize: %d Kb\n", atoi(filesize) / 1024);
                        printf("Percent : %d%% ( %d Kb)\n",count1 / percent ,count1 / 1024);
                        count1+=percent;
                    }
                    count2++;
                    received++;
                    recvBUFF[0] = 0;
                } else {
                close(listenSOCKET);
                return 0;
            }
            }
        close(listenSOCKET);
        } else {
        printf("Client dropped connection\n");
        }

    return 0;
    }
}