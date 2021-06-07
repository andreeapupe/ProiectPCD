//
// Created by mihalic on 07.06.2021.
//

#ifndef PROIECT_WEB_H
#define PROIECT_WEB_H


#include <stdbool.h>
#include <string.h>

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

int findMatch(char text[], char pattern[]) {
    int c, d, e, text_length, pattern_length, position = -1;

    text_length    = strlen(text);
    pattern_length = strlen(pattern);

    if (pattern_length > text_length) {
        return -1;
    }

    for (c = 0; c <= text_length - pattern_length; c++) {
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

void reqToStruct(char req[])
{
    struct reqBody {
        char key[64];
        char value[64];
    };
    struct returnExample{
        char method[6]; //POST/GET
        struct reqBody bdy[10];
    };

   struct returnExample structura;
   char type[5];
   if(checkHttpReqType(req)==1)
       strncpy(structura.method,req,4);
   if(checkHttpReqType(req)==2)
       strncpy(structura.method,req,3);

}





#endif //PROIECT_WEB_H
