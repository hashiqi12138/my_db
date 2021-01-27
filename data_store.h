//
// Created by liujie on 2020/3/20.
//

#ifndef MY_DB_DATA_STORE_H
#define MY_DB_DATA_STORE_H

#include <stdint.h>

#define COLUMN_USERNAME_SIZE 32  //username字段的大小
#define COLUMN_EMAIL_SIZE 255    //email字段的大小
#define TABLE_MAX_PAGES 100      //表最大的page数
#define PAGE_SIZE 4096           //直接引用或运算不会发生错误

/**
 * 枚举数据执行的结果
 * */
typedef enum { EXECUTE_SUCCESS, EXECUTE_TABLE_FULL } ExecuteResult;

/**
 * 数据库中表征表一行的结构体
 * */
typedef struct ROW{
    uint32_t id;
    char username[COLUMN_USERNAME_SIZE + 1];
    char email[COLUMN_EMAIL_SIZE + 1];
}ROW;

typedef struct _Pager{
    int file_descriptor;
    uint32_t file_length;
    void* pages[TABLE_MAX_PAGES];
} Pager;

Pager* pager_open (const char* filename);

void * get_page (Pager*, int);

void pager_flush(Pager* pager, uint32_t page_num, uint32_t size);

/**
 * 数据库中表征表的结构体
 * */
typedef struct {
    uint32_t num_rows;
    Pager* pager;
    void* pages[TABLE_MAX_PAGES];
} Table;

/**
 * 输出在源文件中定义的各种数据格式的大小和ROW中各元素的偏移量等
 * */
void print_size();

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
void serialize_row(ROW* source, void* destination);

/**
 * @param source {ROW} 要写入page的ROW结构体指针
 * @param destination {void*} 指向要写入的内存起始地址
 * 反序列化，读取page中的每个row，复制到目标row
 * 同样是根据offset
 * */
void deserialize_row(void* source, ROW* destination);

/**
 * @param table {Table} 要读取的表
 * @param row_num {uint32_t} row的index
 * @return table中第row_num个row的起始内存的指针
 * 根据row_num 和 ROWS_PER_PAGE 计算page index  例：0->page_index 0 ; 1->page_index 1 ; 13->page_index 0 ;(中间有25个字节的无用字节); 14->page_index 1
 * 根据row_num 与 ROWS_PER_PAGE求模 计算byte offset 例：0->byte_offset 0 ; (进入下一页,偏移应当为0); 14->byte_offset 0 ; 15->byte_offset 1
 * 返回di row_num 个row应当被存储的内存起始地址的指针
 * */
void* row_slot(Table* table, uint32_t row_num);


/**
 * 打开文件，创建table
 * @param table
 * @return
 */
Table* db_open(const char* filename);

/**
 * 程序退出前，将数据写到文件
 * 释放文件句柄
 * @param table
 */
void db_close(Table* table);

/**
 * @param table {Table}
 * 释放table，先释放table中指针数组pages每个元素所指向的内存
 * */
void free_table(Table* table);

/**
 * @param table {Table*}
 * @param row {ROW*}
 * @return {ExecuteResult}
 * 向table指针指向的table中插入一行
 * 先序列化复制到table的对应page中
 * 将num_rows+1
 * */
ExecuteResult insert_row(Table*,ROW*);

/**
 * @param table {Table*}
 * @return {ExecuteResult}
 * 读取table中的所有行
 * 根据table的num_rows，将每一个row的地址取到
 * 根据取到的地址将page的值复制到row
 * 打印row的内容
 * */
ExecuteResult select_row(Table*);

/**
 * @param row {ROW*}
 * 打印row的内容
 * */
void print_row(ROW* row);

#endif //MY_DB_DATA_STORE_H
