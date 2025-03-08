cmake_minimum_required(VERSION 3.29)
project(VersatileCInterpreter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_TOOLCHAIN_FILE "/home/max/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")

find_package(GTest REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/generated)

add_executable(VersatileCInterpreter
        src/main.cpp
        src/CInterpreterVisitor.cpp
        src/CInterpreterVisitor.h
        src/Interpreter.cpp
        src/Interpreter.h
        generated/CParser.cpp
        generated/CLexer.cpp
        generated/CBaseVisitor.cpp
        generated/CVisitor.cpp
        src/REPL.cpp
        src/REPL.h
        src/CustomErrorListener.cpp
        src/CustomErrorListener.h
        src/Environment.cpp
        src/Environment.h
        src/Variable.h
        src/Utils.h
)

target_link_libraries(VersatileCInterpreter PRIVATE antlr4_static)

# Include tests
add_subdirectory(tests)
