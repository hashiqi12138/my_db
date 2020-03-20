//
// Created by liujie on 2020/3/18.
// 虚拟机，处理数据指令如select insert delete
//
#include <stdio.h>
#include <stdlib.h>
#include "command_processor.h"
#include "data_store.h"

#ifndef MY_DB_VIRTUAL_MACHINE_H
#define MY_DB_VIRTUAL_MACHINE_H


/**
 * @param statement {Statement}  要处理的指令的类型
 * @return {void}
 * 处理对应的数据指令
 * */
ExecuteResult execute_command(Statement*);

#endif //MY_DB_VIRTUAL_MACHINE_H
