// Lab 4: hexdump.c - utility functions to display memory content ranges - C Part 2 Lect

// See: http://stackoverflow.com/questions/7775991/how-to-get-hexdump-of-a-structure-data

#include <stdio.h>
#include "common_functions.h"

// REUSABLE FUNCTION TO READ A STRING
char *read_string(int socket){

        size_t payload_length = sizeof(size_t);
        readn(socket, (unsigned char *) &payload_length, sizeof(size_t));

        //printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);//DEBUG

        // CALCULATE AND ALLOCATE MEMORY FOR THE RESULT
        size_t memallocation = (sizeof(char)*payload_length)+1;
        char *result = (char *)malloc(memallocation);

        if(result!=NULL){

        // INITIALISE EVERY PART OF MEM - WITHOUT WAS CAUSING VALG ISSUES
        memset(result,'\0',memallocation);

        readn(socket, (unsigned char *) result, payload_length);

        //printf("PAYLOAD: %zu %zu//EOL\n",payload_length,n);//DEBUG

        // PRINT STRING TO CONSOLE
        //printf("%s\n",result);

        return result;

        // FREE UP THE RESULT
        //free(result);
        //result = NULL;

        } else {

                return NULL;

        }

}

// REUSABLE FUNCTION TO SEND A STRING BACK TO CLIENT
void send_string(int socket, char *response){

        size_t payload_length = strlen(response);

        //printf("PAYLOAD: %s //EOL%zu\n",response,payload_length);//DEBUG

        writen(socket, (unsigned char *) &payload_length, sizeof(size_t));

        //printf("DATA: %s //EOL%zu\n",response,payload_length);//dEBUG

        writen(socket, (unsigned char *)response, payload_length);

//      free(response);

}
