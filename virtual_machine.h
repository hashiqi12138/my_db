//
// Created by liujie on 2020/3/18.
// 虚拟机，处理数据指令如select insert delete
//

#include "command_processor.h"

#ifndef MY_DB_VIRTUAL_MACHINE_H
#define MY_DB_VIRTUAL_MACHINE_H

/**
 * @param statement {Statement}  要处理的指令的类型
 * @return {void}
 * 处理对应的数据指令
 * */
void execute_command(Statement*);

#endif //MY_DB_VIRTUAL_MACHINE_H
