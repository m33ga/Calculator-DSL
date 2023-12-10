cmake_minimum_required(VERSION 3.26)
project(calculator_dsl)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(calculator_dsl
        lexer.h
        parser.h
        interpreter.h
        main.cpp)
