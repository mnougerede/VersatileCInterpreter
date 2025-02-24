cmake_minimum_required(VERSION 3.29)
project(VersatileCInterpreterTests)

# Enable testing in CMake
enable_testing()

# Find GoogleTest
find_package(GTest REQUIRED)

# Define the test executable
add_executable(VersatileCInterpreterTests
        main.cpp
        CInterpreterVisitorTests.cpp
        InterpreterTests.cpp
        REPLTests.cpp

        ${CMAKE_SOURCE_DIR}/src/Interpreter.cpp
        ${CMAKE_SOURCE_DIR}/src/REPL.cpp
        ${CMAKE_SOURCE_DIR}/src/CInterpreterVisitor.cpp
        ${CMAKE_SOURCE_DIR}/src/CustomErrorListener.cpp
        ${CMAKE_SOURCE_DIR}/src/Environment.cpp

        ${CMAKE_SOURCE_DIR}/generated/CLexer.cpp
        ${CMAKE_SOURCE_DIR}/generated/CParser.cpp
        EnvironmentTests.cpp
)

target_include_directories(VersatileCInterpreterTests PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/generated
        /home/max/.vcpkg-clion/vcpkg/installed/x64-linux/include/antlr4-runtime
)

# Locate the ANTLR4 runtime library from your vcpkg installation.
find_library(ANTLR4_RUNTIME_LIB antlr4-runtime
        PATHS /home/max/.vcpkg-clion/vcpkg/installed/x64-linux/lib
)
if(NOT ANTLR4_RUNTIME_LIB)
    message(FATAL_ERROR "ANTLR4 runtime library not found")
endif()

# Link GoogleTest
target_link_libraries(VersatileCInterpreterTests
        PRIVATE
            GTest::gtest_main
            ${ANTLR4_RUNTIME_LIB}
)

# Register tests with CTest
add_test(NAME VersatileCInterpreterTests COMMAND VersatileCInterpreterTests)
