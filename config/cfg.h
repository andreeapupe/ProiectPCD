#define PORT_STD_CLIENT 17555
#define PORT_ADM_CLIENT 18555
#define PORT_WEB_CLIENT 8080
#define PORT_WEB_CLIENT_ALTERNATIVE 9091
#define MAX_STD_CLIENT_NUMBER 10
#define FILE_SIZE_CHUNK 1024

#define MAC 0

#define TRUE 1
#define FALSE 0

static char defaultHttpResponse[2048] = "HTTP/1.1 200 OK\ncontent-type: text/html";

static const char xmlSendList[10][45]={
    "./client -send /simulation/car1.xml",
    "./client -send /simulation/car2.xml",
    "./client -send /simulation/car3.xml",
    "./client -send /simulation/car4.xml",
    "./client -send /simulation/car5.xml",
    "./client -send /simulation/car6.xml",
    "./client -send /simulation/car7.xml",
    "./client -send /simulation/car8.xml",
    "./client -send /simulation/car9.xml",
    "./client -send /simulation/car10.xml"
}
static const char xmlGetList[10][45]={
    "./client -get /simulation/car1.xml",
    "./client -get /simulation/car2.xml",
    "./client -get /simulation/car3.xml",
    "./client -get /simulation/car4.xml",
    "./client -get /simulation/car5.xml",
    "./client -get /simulation/car6.xml",
    "./client -get /simulation/car7.xml",
    "./client -get /simulation/car8.xml",
    "./client -get /simulation/car9.xml",
    "./client -get /simulation/car10.xml"
}