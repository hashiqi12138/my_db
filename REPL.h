//
// Created by liujie on 2020/3/17.
// encoding utf-8
//
#include <stddef.h>
#ifndef MY_DB_REPL_H
#define MY_DB_REPL_H
    /**
     * 输入缓冲结构体，保存从命令行读取的一串字符串
     * */
    typedef struct _in_buffer{
        char* buffer;          //字符串
        size_t buffer_length;  //缓冲的大小
        size_t input_length;   //读取的字符串长度
    } InputBuffer;

    /**
    * 初始化一个InputBuffer
    * @return {InputBuffer}
    * */
    InputBuffer* new_in_buffer();

    /**
    * 读取命令行字符到InputBuffer
    * @param in_buffer {InputBuffer} 待写入的InputBuffer
    * @return void
    * */
    void read_input(InputBuffer* in_buffer);

    /**
    * 释放一个InputBuffer的内存空间
    * @param in_buffer {InputBuffer} 一个InputBuffer
    * @return void
    * */
    void close_input_buffer(InputBuffer* in_buffer);

    /**
     * 输出固定的开头
     * @return void
     * */
    void format_prompt_out();

    /**
     * 处理不同的命令行指令
     * @param in_buffer {InputBuffer} 获取到的一行字符串
     * @return void
     * */
    void dispatch_command(InputBuffer* in_buffer);
#endif //MY_DB_REPL_H
