cmake_minimum_required(VERSION 3.15)
project(my_db C)

set(CMAKE_C_STANDARD 11)

add_executable(my_db main.c  REPL.c REPL.h)