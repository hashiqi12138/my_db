//
// Created by liujie on 2020/3/18.
//
#include <stdlib.h>
#include <stdio.h>
#include "virtual_machine.h"

/**
 * @param statement {Statement}  要处理的指令的类型
 * @return {void}
 * 处理对应的数据指令
 * */
void execute_command(Statement* statement){
    switch (statement->type) {
        case (STATEMENT_INSERT):
            printf("This is where we would do an insert.\n");
            break;
        case (STATEMENT_SELECT):
            printf("This is where we would do a select.\n");
            break;
    }
};