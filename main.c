
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "REPL.h"

#define TRUE 1
#define FALSE 0

Table* table;

int format_output(char *);

int main(int argc, char* argv[]) {

    if (argc < 2) {
        printf("Must supply a database filename. \n");
        exit(EXIT_FAILURE);
    }
    table = db_open(argv[1]);
    InputBuffer *in_buffer = new_in_buffer();
    int flag=TRUE;
    int count=1000;
    printf("____________welcom to my_db_____________\n");
    while(flag){
        format_prompt_out(in_buffer);
        read_input(in_buffer);
        dispatch_command(in_buffer);
    }

    return 0;
}




