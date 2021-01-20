//
// Created by liujie on 2020/3/18.
//
#include <stdlib.h>
#include <stdio.h>
#include "virtual_machine.h"
#include "data_store.h"

/**
 * @param statement {Statement}  要处理的指令的类型
 * @return {void}
 * 处理对应的数据指令
 * */
ExecuteResult execute_command(Statement* statement, Table* table){
    switch (statement->type) {
        case (STATEMENT_INSERT):
            printf("This is where we would do an insert.\n");
            return handle_insert(statement, table);
        case (STATEMENT_SELECT):
            printf("This is where we would do a select.\n");
            return handle_select(statement, table);
        case (STATEMENT_CREATE):
            printf("This is where we would do a create.\n");
            break;
    }
};

/**
 * @param statement{Statement}  处理后的insert指令，包含一个row_to_insert(待插入的行)
 * 将处理完成的insert 命令交给虚拟机执行，返回虚拟机执行的结果
 * */
ExecuteResult handle_insert(Statement* statement, Table* table){
    return insert_row(table,&(statement->row_to_insert));
}

/**
 * @param statement{Statement}  处理后的select指令
 * 将处理完成的select 命令交给虚拟机执行，返回虚拟机执行的结果
 * */
ExecuteResult handle_select(Statement* statement, Table* table){
    //printf("table row count is %d\n",table->num_rows);
    return select_row(table);
}