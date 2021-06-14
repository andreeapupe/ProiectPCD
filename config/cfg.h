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

static const char FORK_ERROR_MESSAGE[256] = "[-] Cannot fork";
static const char LISTEN_WEB_ERROR_MESSAGE[256] = "[-] Listen error WEB CLIENT";
static const char LISTEN_STD_ERROR_MESSAGE[256] = "[-] Listen error STD CLIENT";
static const char ACCEPT_ERROR_MESSAGE[256] = "[-] Accept failure";
