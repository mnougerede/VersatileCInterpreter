#include "gtest/gtest.h"
#include "Environment.h"  // Adjust the include path as needed
#include <stdexcept>

// Test that setting and getting a variable in the global environment works.
TEST(EnvironmentTest, SetAndGetGlobalVariable) {
    Environment globalEnv;
    // Set a variable named "x" of type INT with value 42.
    globalEnv.define("x", VarType::INT, 42);

    // Get the variable and check its value.
    Variable var = globalEnv.get("x");
    EXPECT_EQ(var.type, VarType::INT);
    // Since we stored an int, we use std::get<int> on the variant.
    EXPECT_EQ(std::get<int>(var.value), 42);
}

// Test that exists() returns true for defined variables and false for undefined.
TEST(EnvironmentTest, VariableExists) {
    Environment globalEnv;
    globalEnv.define("y", VarType::DOUBLE, 3.14);

    EXPECT_TRUE(globalEnv.exists("y"));
    EXPECT_FALSE(globalEnv.exists("z"));
}

// Test that a nested environment inherits variables from its parent.
TEST(EnvironmentTest, NestedEnvironmentInheritance) {
    Environment globalEnv;
    globalEnv.define("a", VarType::INT, 100);

    // Create a nested environment with globalEnv as parent.
    Environment localEnv(&globalEnv);
    // Local environment does not override "a", so it should be inherited.
    Variable var = localEnv.get("a");
    EXPECT_EQ(var.type, VarType::INT);
    EXPECT_EQ(std::get<int>(var.value), 100);
}

// Test that a variable set in a nested environment shadows the parent.
TEST(EnvironmentTest, NestedEnvironmentShadowing) {
    Environment globalEnv;
    globalEnv.define("b", VarType::INT, 10);

    Environment localEnv(&globalEnv);
    // Set a variable "b" in the local environment with a different value.
    localEnv.define("b", VarType::INT, 20);

    // Lookup "b" in local environment should return the local value.
    Variable localVar = localEnv.get("b");
    EXPECT_EQ(std::get<int>(localVar.value), 20);

    // The global environment should still have the original value.
    Variable globalVar = globalEnv.get("b");
    EXPECT_EQ(std::get<int>(globalVar.value), 10);
}

// Test that attempting to get an undefined variable throws an exception.
TEST(EnvironmentTest, UndefinedVariableThrows) {
    Environment globalEnv;
    EXPECT_THROW({
        globalEnv.get("undefined_var");
    }, std::runtime_error);
}
