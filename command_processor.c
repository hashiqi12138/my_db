//
// Created by liujie on 2020/3/18.
// 处理命令行传入的指令
//
#include "command_processor.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "data_store.h"

Table* table;

/**
 * @param in_buffer {InputBuffer} 命令行输入的一行指令
 * @return {META_COMMAND_RESULT} 返回处理的结果，成功或未识别
 * 元指令预处理，输入未经处理得指令，返回该元指令是否是正确的指令
 * */
META_COMMAND_RESULT handle_meta_command(InputBuffer* in_buffer){
    if (strcmp(in_buffer->buffer, ".exit") == 0) {
        close_input_buffer(in_buffer);
        free_table(table);
        exit(EXIT_SUCCESS);
    } else {
        return UNRECOGNIZED_META_COMMAND;
    }
};

/**
 * @param in_buffer {InputBuffer} 命令行输入的一行指令
 * @param statement {Statement}  要写入的指令类型的指针
 * @return {META_COMMAND_RESULT} 返回指令的类型 select insert delete unrecognized
 * 其他指令预处理，将分析后的指令类型赋值给statement指向的地址，返回预处理的结果，
 * 若是可识别的指令则返回success
 * 否则返回unrecognized
 * */
PREPARE_RESULT prepare_statement(InputBuffer* in_buffer,Statement* statement){
    if (strncmp(in_buffer->buffer, "insert", 6) == 0) {
        statement->type = STATEMENT_INSERT;
        int args_assigned = sscanf(
                in_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),
                statement->row_to_insert.username, statement->row_to_insert.email);
        if (args_assigned < 3) {
            return PREPARE_SYNTAX_ERROR;
        }
        return PREPARE_SUCCESS;
    }
    if (strncmp(in_buffer->buffer, "select",6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    if (strncmp(in_buffer->buffer, "create",6) == 0) {
        statement->type = STATEMENT_CREATE;
        return PREPARE_SUCCESS;
    }
    return UNRECOGNIZED_PREPARE_STATEMENT;
};

