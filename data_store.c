//
// Created by liujie on 2020/3/20.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include "data_store.h"
#include "cursor.h"


#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)    //输出结构体中属性占用的内存大小
const uint32_t ID_SIZE = size_of_attribute(ROW, id);                            //id的大小
const uint32_t USERNAME_SIZE = size_of_attribute(ROW, username);                //username的大小
const uint32_t EMAIL_SIZE = size_of_attribute(ROW, email);                      //email的大小
const uint32_t ID_OFFSET = 0;                                                   //id在序列化时的相对首地址的偏移字节数
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE;
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES;


void print_size() {
    printf("ID size is  %d\n", ID_SIZE);
    printf("Email size is  %d\n", EMAIL_SIZE);
    printf("username size is  %d\n", USERNAME_SIZE);
    printf("ID offset is  %d\n", ID_OFFSET);
    printf("Email offset is  %d\n", EMAIL_OFFSET);
    printf("username offset is  %d\n", USERNAME_OFFSET);
    printf("ROW size is  %d\n", ROW_SIZE);
    printf("rows per page is %d\n", ROWS_PER_PAGE);
    printf("table max rows is %d\n", TABLE_MAX_ROWS);
}

/**
 * @param source {ROW} 要写入page的ROW结构体指针
 * @param destination {void*} 指向要写入的内存起始地址
 * 将row的各个属性值按字节根据源文件中定义的各个字段的offset
 * 依次复制到page对应的内存空间
 * 例 int       id     offset 0        size  4       //复制源 row 0-3 字节到目标地址
 *    char[32]  name   offset 4        size 32       //复制源row  4-31字节到目标地址
 *    char[255] email  offset 36(4+32) size 255      //复制源row  32-290字节到目标地址
 *    total                            size 291       //相邻row之间的内存偏移应当是291   内存起始地址应当是0 291 582....
 * */
void serialize_row(ROW *source, void *destination) {
    memcpy((char *) destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy((char *) destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy((char *) destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/**
 * @param source {ROW} 要写入page的ROW结构体指针
 * @param destination {void*} 指向要写入的内存起始地址
 * 反序列化，读取page中的每个row，复制到目标row
 * 同样是根据offset
 * */
void deserialize_row(void *source, ROW *destination) {
    memcpy(&(destination->id), (char *) source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), (char *) source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), (char *) source + EMAIL_OFFSET, EMAIL_SIZE);
}

/**
 * @param table {Table} 要读取的表
 * @param row_num {uint32_t} row的index
 * @return table中第row_num个row的起始内存的指针
 * 根据row_num 和 ROWS_PER_PAGE 计算page index  例：0->page_index 0 ; 1->page_index 1 ; 13->page_index 0 ;(中间有25个字节的无用字节); 14->page_index 1
 * 根据row_num 与 ROWS_PER_PAGE求模 计算byte offset 例：0->byte_offset 0 ; (进入下一页,偏移应当为0); 14->byte_offset 0 ; 15->byte_offset 1
 * 返回di row_num 个row应当被存储的内存起始地址的指针
 * */
void *row_slot(Table *table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(table->pager, page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    //TODO 直接引用ROW_SIZE进行运算会出现不正确、特别大的值，替代方案
    uint32_t row_size = ROW_SIZE;
    uint32_t byte_offset = row_offset * row_size;
    return (char *) page + byte_offset;
}

void *cursor_value(Cursor *cursor) {
    uint32_t row_num = cursor->row_num;
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(cursor->table->pager, page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t row_size = ROW_SIZE;
    uint32_t byte_offset = row_offset * row_size;
    return page + byte_offset;
}

Table *db_open(const char *filename) {
    Pager *pager = pager_open(filename);
    uint32_t num_rows = pager->file_length / ROW_SIZE;
    Table *table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;
    return table;
}

void db_close(Table *table) {
    Pager *pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;
    for (uint32_t i = 0; i < num_full_pages; i++) {
        if (pager->pages[i] == NULL) {
            continue;
        }
        pager_flush(pager, i, PAGE_SIZE);
        printf("page size is %d\n", PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree

    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0) {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL) {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file_descriptor);
    if (result == -1) {
        printf("Error closing db file. \n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = pager->pages[i];
        if (page) {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
};

/**
 * @param table {Table}
 * 释放table，先释放table中指针数组pages每个元素所指向的内存
 * */
void free_table(Table *table) {
    for (int i = 0; table->pages[i]; i++) {
        free(table->pages[i]);
    }
    free(table);
}

/**
 * @param table {Table*}
 * @param row {ROW*}
 * @return {ExecuteResult}
 * 向table指针指向的table中插入一行
 * 先序列化复制到table的对应page中
 * 将num_rows+1
 * */
ExecuteResult insert_row(Table *table, ROW *row) {
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }
    Cursor *cursor = table_end(table);
    serialize_row(row, cursor_value(cursor));
    table->num_rows += 1;
    free(cursor);
    return EXECUTE_SUCCESS;
};

/**
 * @param table {Table*}
 * @return {ExecuteResult}
 * 读取table中的所有行
 * 根据table的num_rows，将每一个row的地址取到
 * 根据取到的地址将page的值复制到row
 * 打印row的内容
 * */
ExecuteResult select_row(Table *table) {
    ROW row;
    Cursor* cursor = table_start(table);
    while (!cursor->end_of_table) {
        deserialize_row(cursor_value(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }
    free(cursor);
    return EXECUTE_SUCCESS;
};

/**
 * @param row {ROW*}
 * 打印row的内容
 * */
void print_row(ROW *row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}

Pager *pager_open(const char *filename) {
    int file_handle = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (file_handle == -1) {
        printf("Unable to open file: %s\n", filename);
        exit(EXIT_FAILURE);
    }
    off_t file_length = lseek(file_handle, 0, SEEK_END);
    Pager *pager = malloc(sizeof(Pager));
    pager->file_descriptor = file_handle;
    pager->file_length = file_length;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        pager->pages[i] = NULL;
    }
    return pager;
}

void *get_page(Pager *pager, int page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Try to fetch page number out of bounds. %d > %d\n", page_num, TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }
    if (pager->pages[page_num] == NULL) {
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;

        if (pager->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d \n", errno);
                exit(EXIT_FAILURE);
            }
        }
        pager->pages[page_num] = page;
    }
    return pager->pages[page_num];
}


void pager_flush(Pager *pager, uint32_t page_num, uint32_t size) {
    if (pager->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written = write(pager->file_descriptor, pager->pages[page_num], size);

    if (bytes_written == -1) {
        printf("Error writing: %d \n", errno);
        exit(EXIT_FAILURE);
    }
};
