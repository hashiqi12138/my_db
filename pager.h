//
// Created by liujie52 on 2021/1/27.
//

#include <stdint.h>

#ifndef MY_DB_PAGER_H
#define MY_DB_PAGER_H

#define TABLE_MAX_PAGES 100      //表最大的page数
#define PAGE_SIZE 4096           //直接引用或运算不会发生错误


typedef struct _Pager{
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;

Pager* pager_open (const char* filename);

void * get_page (Pager*, int);

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);

#endif //MY_DB_PAGER_H
