//
// Created by liujie on 2020/3/17.
// encoding utf-8
//
#include <stdio.h>
#include <stdlib.h>
#include "REPL.h"
#include "command_processor.h"
#include "virtual_machine.h"
#include "data_store.h"

extern Table *table;

/**
 * @param line {char**} 指向一个动态分配的char类型的指针，可以为空
 * @param size_t {size_t} 指向的内存空间的长度，字节单位
 * @param FILE {FILE} 标准输入 stdin
 * @return {size_t} 读取的字符串长度，包括'\0'结束符
 * 动态分配内存空间，读取字符直到遇到EOF
 * */

size_t get_line(char **line, size_t *n, FILE *fp) {
    char *buf = *line;
    size_t c, i = 0;//i来记录字符串长度，c来存储字符
    if (buf == NULL || *n == 0)//如果指向的内存为空指针或读取的字节长度为0，则分配10字节的内存空间
    {
        *line = malloc(10);
        buf = *line;
        *n = 10;
    }
    //buf为或n为0时动态为期分配空间
    while ((c = fgetc(fp)) != '\n') {
        if (c == EOF)
            return -1;
        if (i < *n - 1)//留2个空间给\n和\0
        {
            *(buf + i++) = c;
        } else {
            *n = *n + 10;
            buf = realloc(buf, *n);//空间不足时，重新进行分配
            *line = buf;//使line指向新分配的地址
            *(buf + i++) = c;
        }
    }
    //*(buf+i++)='\n';
    *(buf + i) = '\0';
    //*(buf+i++)='\n';
    return i;
}

/**
 * @param {void}
 * @return in_buffer {InputBuffer*}
 * 初始化一个InputBuffer
 * */
InputBuffer *new_in_buffer() {
    InputBuffer *in_buffer = malloc(sizeof(InputBuffer));
    in_buffer->buffer = NULL;
    in_buffer->buffer_length = 0;
    in_buffer->input_length = 0;
    return in_buffer;
};

/**
 * @param in_buffer {InputBuffer} 获取到的一行字符串
 * @return void
 * 获取一行指令
 * */
void read_input(InputBuffer *in_buffer) {
    size_t size = get_line(&(in_buffer->buffer), &(in_buffer->buffer_length), stdin);
    if (in_buffer->buffer_length < 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }
};

/**
 * @param in_buffer {InputBuffer} 获取到的一行字符串
 * @return void
 * 释放InputBuffer的内存
 * */
void close_input_buffer(InputBuffer *in_buffer) {
    in_buffer->buffer = NULL;
    //free(in_buffer->buffer);
    free(in_buffer);
};

/**
 * @return void
 * 输出固定的开头
 * */
void format_prompt_out() {
    printf("my_db:> ");
};

/**
 * @param in_buffer {InputBuffer} 获取到的一行字符串
 * @return void
 * 处理不同的命令行指令
 * */
void dispatch_command(InputBuffer *in_buffer) {
    if (in_buffer->buffer[0] == '.') {
        switch (handle_meta_command(in_buffer, table)) {
            case (META_COMMAND_SUCCESS):
                break;
            case (UNRECOGNIZED_META_COMMAND):
                printf("Unrecognized command '%s'\n", in_buffer->buffer);
                break;
        }
    } else {
        Statement statement;
        switch (prepare_statement(in_buffer, &statement)) {
            case (PREPARE_SUCCESS):
                switch (execute_command(&statement, table)) {
                    case EXECUTE_SUCCESS:
                        printf("Execute Success !\n");
                        break;
                    case EXECUTE_TABLE_FULL:
                        printf("Table is full, row not inserted !\n");
                        break;
                }
                break;
            case (PREPARE_STRING_TOO_LONG):
                printf("String is out of field length !\n");
            case PREPARE_NEGATIVE_ID:
                printf("ID must be POSITIVE !\n");
            case PREPARE_SYNTAX_ERROR:
                printf("Syntax error. Could not parse statement \n");
                break;
            case (UNRECOGNIZED_PREPARE_STATEMENT):
                printf("Unrecognized keyword at start of '%s'.\n", in_buffer->buffer);
                break;
        }
    }
};

