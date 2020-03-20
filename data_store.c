//
// Created by liujie on 2020/3/20.
//
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "data_store.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)    //输出结构体中属性占用的内存大小
const uint32_t ID_SIZE = size_of_attribute(ROW, id);                            //id的大小
const uint32_t USERNAME_SIZE = size_of_attribute(ROW, username);                //username的大小
const uint32_t EMAIL_SIZE = size_of_attribute(ROW, email);                      //email的大小
const uint32_t ID_OFFSET = 0;                                                   //id在序列化时的相对首地址的偏移字节数

#define USERNAME_OFFSET ID_OFFSET + ID_SIZE           //直接引用或运算不会发生错误   username在序列化时的相对首地址的偏移字节数
#define EMAIL_OFFSET USERNAME_OFFSET + USERNAME_SIZE  //直接引用或运算不会发生错误   email在序列化时的相对首地址的偏移字节数
#define ROW_SIZE ID_SIZE + USERNAME_SIZE + EMAIL_SIZE //直接引用不会发生错误 运算后值怪异

#define PAGE_SIZE 4096                                //直接引用或运算不会发生错误

#define TABLE_MAX_PAGES 100                           //直接引用或运算不会发生错误
//#define ROWS_PER_PAGE PAGE_SIZE / ROW_SIZE          //直接引用不会发生错误 运算后值怪异
//#define TABLE_MAX_ROWS ROWS_PER_PAGE * TABLE_MAX_PAGES  //直接引用不会发生错误 运算后值怪异

//替代方案 ROWS_PER_PAGE
uint32_t ROWS_PER_PAGE(){
    uint32_t page_size=PAGE_SIZE;
    uint32_t row_size=ROW_SIZE;
    uint32_t result=page_size/row_size;
    return result;
}

//替代方案 TABLE_MAX_ROWS
uint32_t TABLE_MAX_ROWS(){
    uint32_t rows_per_page=ROWS_PER_PAGE();
    uint32_t table_max_pages=TABLE_MAX_PAGES;
    uint32_t result=rows_per_page*table_max_pages;
    return result;
}

void print_size(){
    printf("ID offset is  %d\n",ID_OFFSET);
    printf("Email offset is  %d\n",EMAIL_OFFSET);
    printf("username offset is  %d\n",USERNAME_OFFSET);
    printf("ROW size is  %d\n",ROW_SIZE);
    printf("rows per page is %d\n",ROWS_PER_PAGE());
    printf("table max rows is %d\n",TABLE_MAX_ROWS());
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
void serialize_row(ROW* source, void* destination) {
    memcpy((char*)destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy((char*)destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy((char*)destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

/**
 * @param source {ROW} 要写入page的ROW结构体指针
 * @param destination {void*} 指向要写入的内存起始地址
 * 反序列化，读取page中的每个row，复制到目标row
 * 同样是根据offset
 * */
void deserialize_row(void* source, ROW* destination) {
    memcpy(&(destination->id), (char*)source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), (char*)source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), (char*)source + EMAIL_OFFSET, EMAIL_SIZE);
}

/**
 * @param table {Table} 要读取的表
 * @param row_num {uint32_t} row的index
 * @return table中第row_num个row的起始内存的指针
 * 根据row_num 和 ROWS_PER_PAGE 计算page index  例：0->page_index 0 ; 1->page_index 1 ; 13->page_index 0 ;(中间有25个字节的无用字节); 14->page_index 1
 * 根据row_num 与 ROWS_PER_PAGE求模 计算byte offset 例：0->byte_offset 0 ; (进入下一页,偏移应当为0); 14->byte_offset 0 ; 15->byte_offset 1
 * 返回di row_num 个row应当被存储的内存起始地址的指针
 * */
void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num=row_num/ROWS_PER_PAGE();
    void* page = table->pages[page_num];
    if (page == NULL) {
        // Allocate memory only when we try to access page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE();
    //TODO 直接引用ROW_SIZE进行运算会出现不正确、特别大的值，替代方案
    uint32_t row_size=ROW_SIZE;
    uint32_t byte_offset = row_offset * row_size;
    return (char*)page + byte_offset;
}

/**
 * @return {Table}
 * 生成table，并初始化，将每一页都置为NULL，只有但请求对应page时才对其分配内存
 * */
Table* new_table() {
    Table* table = malloc(sizeof(Table));
    table->num_rows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        table->pages[i] = NULL;
    }
    return table;
}

/**
 * @param table {Table}
 * 释放table，先释放table中指针数组pages每个元素所指向的内存
 * */
void free_table(Table* table) {
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
ExecuteResult insert_row(Table* table,ROW* row){
    if (table->num_rows >= TABLE_MAX_ROWS()) {
        return EXECUTE_TABLE_FULL;
    }
    serialize_row(row, row_slot(table, table->num_rows));
    table->num_rows += 1;
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
ExecuteResult select_row(Table* table){
    ROW row;
    for (uint32_t i = 0; i < table->num_rows; i++) {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
};

/**
 * @param row {ROW*}
 * 打印row的内容
 * */
void print_row(ROW* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
