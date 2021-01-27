//
// Created by liujie52 on 2021/1/27.
//

#ifndef MY_DB_CURSOR_H
#define MY_DB_CURSOR_H

typedef int bool;

#define TRUE 1

#define FALSE 0

#include "data_store.h"

typedef struct {
    Table* table;
    uint32_t row_num;
    bool end_of_table;
} Cursor;

Cursor* table_start(Table*);

Cursor* table_end(Table*);

void cursor_advance(Cursor*);

#endif //MY_DB_CURSOR_H
