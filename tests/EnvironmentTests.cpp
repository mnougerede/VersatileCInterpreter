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

// In EnvironmentTest.cpp

// 1) assign(): writing to an existing variable, both locally and in a parent
TEST(EnvironmentTest, AssignUpdatesLocal) {
    Environment env;
    env.define("x", VarType::INT, 1);
    env.assign("x", VarType::INT, 5);
    EXPECT_EQ(std::get<int>(env.get("x").value), 5);
}

TEST(EnvironmentTest, AssignPropagatesToParent) {
    Environment parent;
    parent.define("y", VarType::INT, 2);
    Environment child(&parent);
    child.assign("y", VarType::INT, 7);
    // child has no own 'y', so parent should change
    EXPECT_EQ(std::get<int>(parent.get("y").value), 7);
}

// 2) assign() on undefined should throw
TEST(EnvironmentTest, AssignUndefinedThrows) {
    Environment env;
    EXPECT_THROW(env.assign("no_such", VarType::INT, 0), std::runtime_error);
}

// 3) pushScope() / popScope() round-trip
TEST(EnvironmentTest, PushPopScope) {
    Environment root;
    root.define("a", VarType::INT, 42);
    Environment* child = root.pushScope();
    // child sees 'a'
    EXPECT_TRUE(child->exists("a"));
    Environment* back = child->popScope();
    EXPECT_EQ(back, &root);
    delete child;
}

// 4) function‐related methods
TEST(EnvironmentTest, DefineAndLookupFunction) {
    Environment env;
    Function f;
    f.returnType     = VarType::INT;            // ← use the VarType enum
    f.parameterNames = {"x","y"};
    // (if you added parameterTypes, e.g. {VarType::INT,VarType::DOUBLE}, set those too)

    env.defineFunction("foo", f);
    EXPECT_TRUE(env.functionExists("foo"));
    Function* p = env.getFunction("foo");
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(p->returnType, VarType::INT);
    EXPECT_EQ(p->parameterNames.size(), 2u);
}

TEST(EnvironmentTest, FunctionLookupInParent) {
    Environment parent;
    Function f;
    f.returnType = VarType::VOID;                 // again, use enum
    parent.defineFunction("bar", f);

    Environment child(&parent);
    EXPECT_TRUE(child.functionExists("bar"));
    EXPECT_EQ(child.getFunction("bar")->returnType, VarType::VOID);
}
TEST(EnvironmentTest, FunctionParameterTypes) {
    Environment env;
    Function f;
    f.returnType      = VarType::INT;
    f.parameterNames  = {"a","b"};
    f.parameterTypes  = {VarType::INT, VarType::DOUBLE};
    env.defineFunction("mix", f);

    Function* p = env.getFunction("mix");
    ASSERT_NE(p, nullptr);
    EXPECT_EQ(p->parameterTypes.size(), 2u);
    EXPECT_EQ(p->parameterTypes[0], VarType::INT);
    EXPECT_EQ(p->parameterTypes[1], VarType::DOUBLE);
}
