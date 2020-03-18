
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "REPL.h"

#define TRUE 1
#define FALSE 0

int format_output(char *);

int main() {
    InputBuffer *in_buffer = new_in_buffer();
    int flag=TRUE;
    printf("____________welcom to my_db_____________\n");
    while(flag){
        format_prompt_out(in_buffer);
        read_input(in_buffer);
        dispatch_command(in_buffer);
    }
    close_input_buffer(in_buffer);
    return 0;
}




