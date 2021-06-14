#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
;
struct reqBody{
    char key[64];
    char value[64];
};
struct returnExample{
    char method[6]; //POST/GET
    struct reqBody bdy[10];
};

int checkHttpReqType(char req[])
{
    if (req[0]=='P' && req[1]=='O' && req[2]=='S' && req[3]=='T')
        return 1;
    if (req[0]=='G' && req[1]=='E' && req[2]=='T')
        return  2;
}

bool checkHttpReq(char req[])
{
    if(checkHttpReqType(req)==1|| checkHttpReqType(req)==2)
        return  true;
    else
        return false;
}

int findMatch(char text[], char pattern[],int start) {
    int c, d, e, text_length, pattern_length, position = -1;

    text_length    = strlen(text);
    pattern_length = strlen(pattern);

    if (pattern_length > text_length) {
        return -1;
    }

    for (c = start; c <= text_length - pattern_length; c++) {
        position = e = c;

        for (d = 0; d < pattern_length; d++) {
            if (pattern[d] == text[e]) {
                e++;
            }
            else {
                break;
            }
        }
        if (d == pattern_length) {
            return position;
        }
    }

    return -1;
}

struct returnExample reqToStruct(char req[])
{

    struct returnExample structura;
    char type[5];
    if(checkHttpReqType(req)==1)
        strncpy(structura.method,&req[0],4);
    if(checkHttpReqType(req)==2)
        strncpy(structura.method,&req[0],3);
    int start = 0,check;
    int index = findMatch(req,"input",start);

    int indexBdy = 0;

    while(start != sizeof (&req)) {
        check = start;
        if (findMatch(req, "input", start) != -1)
        {

            index = findMatch(req, "input", start);
            int indexEqual = findMatch(req,"=",index);
            start = index+1;
            strncpy(structura.bdy[indexBdy].key, &req[index], indexEqual-index);
            int indexAnd = findMatch(req,"&",index);

            strncpy(structura.bdy[indexBdy].value, &req[index+7], indexAnd-indexEqual-1);

        }
        else break;
        if(check != start)
            indexBdy++;
    }
   
    return structura;
}
const char *wd(int year, int month, int day) {
    /* using C99 compound literals in a single line: notice the splicing */
    return ((const char *[])                                         \
          {"Monday", "Tuesday", "Wednesday",                       \
           "Thursday", "Friday", "Saturday", "Sunday"})[           \
      (                                                            \
          day                                                      \
        + ((153 * (month + 12 * ((14 - month) / 12) - 3) + 2) / 5) \
        + (365 * (year + 4800 - ((14 - month) / 12)))              \
        + ((year + 4800 - ((14 - month) / 12)) / 4)                \
        - ((year + 4800 - ((14 - month) / 12)) / 100)              \
        + ((year + 4800 - ((14 - month) / 12)) / 400)              \
        - 32045                                                    \
      ) % 7];
}
void concat(char* msg,int nr)
{
    int length = snprintf( NULL, 0, "%d", nr);
    char *str = (char*)malloc( length + 1 );
    snprintf( str, length + 1, "%d", nr);
    strcat(msg,str);
    free(str);
    // return *msg;
}
char *responseCode(char req[], char code[])
{

    int hours, minutes, seconds, day, month, year;


    time_t now;
    time(&now);

    struct tm *gtime = gmtime(&now);

    hours = gtime->tm_hour;         // get hours since midnight (0-23)
    minutes = gtime->tm_min;        // get minutes passed after the hour (0-59)
    seconds = gtime->tm_sec;        // get seconds passed after a minute (0-59)

    day = gtime->tm_mday;            // get day of month (1 to 31)
    month = gtime->tm_mon + 1;      // get month of year (0 to 11)
    year = gtime->tm_year + 1900;   // get year since 1900

    char *response;
    response = (char*) malloc(2048);
    if(strcmp(code,"200")==0||strcmp(code,"404")==0)
    {
        char *message;
        if(strcmp(code,"200")==0)
            message = "HTTP/1.1 200 OK\n";
        if(strcmp(code,"404")==0)
            message = "HTTP/1.1 404 Not Found\n";
        strcpy(response,message);
        strcat(response, "content-type: text/html\n");
        strcat(response, "Date: ");
        strncat(response, wd(year,month, day),3);
        strcat(response,", ");
        concat(response,day);


        const char * months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        strcat(response," ");
        strncat(response,months[month],3);
        strcat(response," ");
        concat(response,year);
        strcat(response," ");
        concat(response,hours-12);
        strcat(response, ":");
        concat(response, minutes);
        strcat(response, ":");
        concat(response, seconds);
        strcat(response," GMT\nServer: AlphaCar\n");

        int firstIndex = findMatch(req,"Content-Length:",0);
        int secondIndex = findMatch(req,"Connection:",0);
        strncat(response,&req[firstIndex],secondIndex-firstIndex);

        firstIndex = findMatch(req,"Content-Type:",0);
        secondIndex = findMatch(req,"Content-Length:",0);
        strncat(response,&req[firstIndex],secondIndex-firstIndex);

        firstIndex = findMatch(req,"Connection:",0);
        secondIndex = findMatch(req,"Upgrade",0);
        strncat(response,&req[firstIndex],secondIndex-firstIndex);
        strcat(response, "\r\n\r\n");
    }
    if(strcmp(code,"403")==0)
    {
        char *message;
        message = "HTTP/1.1 403 Forbidden\n";
        
        strcpy(response,message);

        strcat(response, "Server: AlphaCar\n");
        strcat(response, "Connection: keep-alive\n");

        strcat(response, "Date: ");
        strncat(response, wd(year,month, day),3);
        strcat(response,", ");
        concat(response,day);


        const char * months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
        strcat(response," ");
        strncat(response,months[month],3);
        strcat(response," ");
        concat(response,year);
        strcat(response," ");
        concat(response,hours-12);
        strcat(response, ":");
        concat(response, minutes);
        strcat(response, ":");
        concat(response, seconds);
        strcat(response," GMT\n");
        strcat(response, "\r\n\r\n");
    }
   // printf("%s", response);

    return (char*)response;
}
