cmake_minimum_required(VERSION 3.29)
project(VersatileCInterpreter)

set(CMAKE_CXX_STANDARD 26)

set(CMAKE_TOOLCHAIN_FILE "[/home/max/dev/vcpkg]/scripts/buildsystems/vcpkg.cmake")
find_package(GTest REQUIRED)

add_executable(VersatileCInterpreter main.cpp
        Interpreter.cpp
        Interpreter.h
        CVisitor.h
        CVisitor.cpp
        CBaseVisitor.h
        CBaseVisitor.cpp)

target_link_libraries(VersatileCInterpreter GTest::gtest)