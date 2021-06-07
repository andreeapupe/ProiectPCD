//
// Created by mihalic on 07.06.2021.
//

#ifndef PROIECT_WEB_H
#define PROIECT_WEB_H


#include <stdbool.h>
#include <string.h>
struct reqBody {
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

   while(start != sizeof req) {
       check = start;
       if (findMatch(req, "input", start) != -1)
       {

           index = findMatch(req, "input", start);
           int indexEqual = findMatch(req,"=",index);
           start = index+1;
           strncpy(structura.bdy[indexBdy].key, &req[index], indexEqual-index);
           int indexAnd = findMatch(req,"&",index);

           strncpy(structura.bdy[indexBdy].value, &req[index+7], indexAnd-index-(indexEqual-index)-1);

       }
       else break;
       if(check != start)
           indexBdy++;
   }
    printf("method: %s\n",structura.method);
    for(int i =0;i<indexBdy;i++)
    printf("%d: %s, %s \n",i,structura.bdy[i].key,structura.bdy[i].value);

    return structura;
}





#endif //PROIECT_WEB_H
