//
// Created by liujie on 2020/3/18.
// 处理命令行传入的指令
//

#ifndef MY_DB_COMMAND_PROCESSOR_H
#define MY_DB_COMMAND_PROCESSOR_H

#include <stdint.h>
#include <stdio.h>
#include "REPL.h"
#include "data_store.h"
/**
 * 枚举类型 元指令的处理结果（元指令为 .exit 等）
 * */
typedef enum{
    META_COMMAND_SUCCESS,      //正确的指令
    UNRECOGNIZED_META_COMMAND  //未识别的指令
}META_COMMAND_RESULT;

/**
 * 枚举类型 其他指令的分析结果
 * */
typedef enum{
    PREPARE_SUCCESS,                //可处理的指令 select insert delete 等
    UNRECOGNIZED_PREPARE_STATEMENT,  //未识别的指令
    PREPARE_SYNTAX_ERROR            //错误的指令
}PREPARE_RESULT;

/**
 * 枚举类型 可识别的指令类型
 * */
typedef enum{
    STATEMENT_INSERT,  //insert 插入指令
    STATEMENT_SELECT,  //select 选择指令
    STATEMENT_CREATE   //create 创建表
} STATEMENT_TYPE;


/**
 * 指令的类型 select insert delete 等
 * */
typedef struct {
    STATEMENT_TYPE type;    //指令类型
    ROW row_to_insert;
} Statement;


/**
 * @param in_buffer {InputBuffer} 命令行输入的一行指令
 * @return {META_COMMAND_RESULT} 返回处理的结果，成功或未识别
 * 元指令预处理，输入未经处理得指令，返回该元指令是否是正确的指令
 * */
META_COMMAND_RESULT handle_meta_command(InputBuffer* in_buffer);

/**
 * @param in_buffer {InputBuffer} 命令行输入的一行指令
 * @param statement {Statement}  要写入的指令类型的指针
 * @return {META_COMMAND_RESULT} 返回指令的类型 select insert delete unrecognized
 * 其他指令预处理，将分析后的指令类型赋值给statement指向的地址，返回预处理的结果，
 * 若是可识别的指令则返回success
 * 否则返回unrecognized
 * */
PREPARE_RESULT prepare_statement(InputBuffer* in_buffer,Statement* statement);

#endif //MY_DB_COMMAND_PROCESSOR_H
