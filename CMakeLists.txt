cmake_minimum_required(VERSION 3.15)
project(my_db C)

set(CMAKE_C_STANDARD 99)

add_executable(my_db main.c
        REPL.c REPL.h
        command_processor.c
        command_processor.h
        virtual_machine.c
        virtual_machine.h
        data_store.c
        data_store.h
        )