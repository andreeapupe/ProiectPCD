#define PORT_STD_CLIENT 17555
#define PORT_ADM_CLIENT 18555
#define PORT_WEB_CLIENT 8080
#define PORT_WEB_CLIENT_ALTERNATIVE 9091

#define MAX_CLIENTS_NUMBER 60
#define FILE_SIZE_CHUNK 1024

#define MAC 0

#define TRUE 1
#define FALSE 0

static char defaultHttpResponse[2048] = "HTTP/1.1 200 OK\ncontent-type: text/html";

static const char FORK_ERROR_MESSAGE[256] = "[-] Cannot fork";
static const char LISTEN_WEB_ERROR_MESSAGE[256] = "[-] Listen error WEB CLIENT";
static const char LISTEN_STD_ERROR_MESSAGE[256] = "[-] Listen error STD CLIENT";
static const char ACCEPT_ERROR_MESSAGE[256] = "[-] Accept failure";
static const char BIND_STD_ERROR_MESSAGE[256] = "[-] Bind error STD CLIENT";
static const char BIND_WEB_ERROR_MESSAGE[256] = "[-] BInd error WEB CLIENT on port 8080";
static const char CREATE_SOCKET_ERROR_MESSAGE[256] = "[-] Cannot create socket";
static const char ASSIGN_SO_REUSEADDR_ERROR_MESSAGE[256] = "[-] Cannot assign SO_REUSEADDR";
static const char LISTEN_ADMIN_ERROR_MESSAGE[256] = "Listen error ADMIN";
static const char BIND_ADMIN_ERROR_MESSAGE[256] = "Bind error ADMIN";
static const char FILE_CREATION_ERROR_MESSAGE[256] = "Error at file creation";
static const char ADMIN_THREAD_ERROR_MESSAGE[256] = "Cannot create admin component thread";

struct arg_struct
{
    int* ptr;
    char shm[1024];
};
