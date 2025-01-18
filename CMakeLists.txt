cmake_minimum_required(VERSION 3.29)
project(VersatileCInterpreter)

set(CMAKE_CXX_STANDARD 26)

add_executable(VersatileCInterpreter main.cpp
        Interpreter.cpp
        Interpreter.h)
