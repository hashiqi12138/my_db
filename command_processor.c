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
        char* keyword = strtok(in_buffer->buffer, " ");
        char* id_string = strtok(NULL, " ");
        char* username = strtok(NULL, " ");
        char* email = strtok(NULL, " ");
        if (id_string == NULL || username == NULL || email == NULL) {
            return PREPARE_SYNTAX_ERROR;
        }
        int id = atoi(id_string);
        if (id < 0) {
            return PREPARE_NEGATIVE_ID;
        }
        if (strlen(username) > COLUMN_USERNAME_SIZE) {
            return PREPARE_STRING_TOO_LONG;
        }
        if (strlen(email) > COLUMN_EMAIL_SIZE) {
            return PREPARE_STRING_TOO_LONG;
        }
        statement->row_to_insert.id = id;
        strcpy(statement->row_to_insert.username, username);
        strcpy(statement->row_to_insert.email, email);
        return PREPARE_SUCCESS;
    }
    if (strncmp(in_buffer->buffer, "select",6) == 0) {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    if (strncmp(in_buffer->buffer, "create",6) == 0) {
        statement->type = STATEMENT_CREATE;
        char* keyword = strtok(in_buffer->buffer, " ");
        char* schema_name = strtok(NULL, " ");
        char* fields = strtok(NULL, " ");
        if (schema_name == NULL || fields == NULL ) {
            return PREPARE_SYNTAX_ERROR;
        }
        if (strlen(schema_name) > 50) {
            return PREPARE_STRING_TOO_LONG;
        }
        if (strlen(fields) > 255) {
            return PREPARE_STRING_TOO_LONG;
        }
        Schema* schema=create_schema(fields);
        schema->name=schema_name;
        statement->schema=schema;
        return PREPARE_SUCCESS;
    }
    return UNRECOGNIZED_PREPARE_STATEMENT;
};

