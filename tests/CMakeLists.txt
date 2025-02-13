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
)

# Link GoogleTest
target_link_libraries(VersatileCInterpreterTests PRIVATE GTest::gtest_main)

# Register tests with CTest
add_test(NAME VersatileCInterpreterTests COMMAND VersatileCInterpreterTests)
