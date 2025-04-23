#include "gtest/gtest.h"
#include "Interpreter.h"
#include <any>
#include <stdexcept>


TEST(InterpreterTest, SimpleExpression) {
    Interpreter interpreter;
    // Given an expression "3 + 4;", we expect the result to be 7.
    std::any result = interpreter.evaluate("3 + 4;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 7);
}

TEST(InterpreterTest, ComplexExpression) {
    Interpreter interpreter;
    // Evaluate a more complex expression: 2 * (3 + 5) - 4 = 12.
    std::any result = interpreter.evaluate("2 * (3 + 5) - 4;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 12);
}

TEST(InterpreterTest, InvalidExpressionThrows) {
    Interpreter interpreter;
    // An invalid expression (e.g., missing operand) should throw an exception.
    EXPECT_THROW({
        interpreter.evaluate("3 + ;",false);
    }, std::exception);
}
// Test: Negative numbers should be handled correctly.
TEST(InterpreterTest, NegativeNumbers) {
    Interpreter interpreter;
    // Expression: -5 + 2 should yield -3.
    std::any result = interpreter.evaluate("-5 + 2;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, -3);
}

// Test: Operator precedence (multiplication before addition).
TEST(InterpreterTest, OperatorPrecedence) {
    Interpreter interpreter;
    // Expression: 3 + 2 * 4 should yield 11 (not 20).
    std::any result = interpreter.evaluate("3 + 2 * 4;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 11);
}

// Test: Parentheses overriding precedence.
TEST(InterpreterTest, Parentheses) {
    Interpreter interpreter;
    // Expression: (3 + 2) * 4 should yield 20.
    std::any result = interpreter.evaluate("(3 + 2) * 4;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 20);
}

// Test: Floating-point arithmetic.
TEST(InterpreterTest, FloatingPointArithmetic) {
    Interpreter interpreter;
    // Expression: 3.5 + 2.5 should yield 6.0.
    std::any result = interpreter.evaluate("3.5 + 2.5;",false);
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, 6.0);
}

// Test: Handling extra whitespace.
TEST(InterpreterTest, ExtraWhitespace) {
    Interpreter interpreter;
    // Expression with extra spaces: "    3   +    4 " should yield 7.
    std::any result = interpreter.evaluate("    3   +    4 ;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 7);
}

// Test: Unmatched parentheses should throw an exception.
TEST(InterpreterTest, UnmatchedParenthesesThrows) {
    Interpreter interpreter;
    // Expression: "3 + (4 - 1" is missing a closing parenthesis.
    EXPECT_THROW({
        interpreter.evaluate("3 + (4 - 1;",false);
    }, std::exception);
}

// Test: Division by zero (if your interpreter is designed to throw).
TEST(InterpreterTest, DivisionByZeroThrows) {
    Interpreter interpreter;
    // Expression: "5 / 0" should throw an exception.
    EXPECT_THROW({
        interpreter.evaluate("5 / 0;",false);
    }, std::exception);
}
// Test that integer division is performed when both operands are int.
TEST(InterpreterTest, IntegerDivision) {
    Interpreter interpreter;
    // Expression: "5 / 2" should yield 2 (integer division).
    std::any result = interpreter.evaluate("5 / 2;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 2);
}
// Test that floating-point division is performed when at least one operand is floating point.
TEST(InterpreterTest, FloatingPointDivision) {
    Interpreter interpreter;
    // Expression: "5 / 2.0" should yield 2.5.
    std::any result = interpreter.evaluate("5 / 2.0;",false);
    double value = std::any_cast<double>(result);
    EXPECT_DOUBLE_EQ(value, 2.5);
}
// Test that a character literal is evaluated correctly.
// For example, "'A'" should yield the character 'A'.
TEST(InterpreterTest, CharLiteral) {
    Interpreter interpreter;
    std::any result = interpreter.evaluate("'A';",false);
    char value = std::any_cast<char>(result);
    EXPECT_EQ(value, 'A');
}
// Test variable declaration and reference in a single evaluation.
// For example: "int a = 10; a + 5" should yield 15.
TEST(InterpreterTest, VariableDeclarationAndReference) {
    Interpreter interpreter;
    std::string code = "int a = 10; a + 5;";
    std::any result = interpreter.evaluate(code,false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 15);
}

// Test persistent environment across multiple evaluations.
// For example, after declaring a variable, it should be available in subsequent evaluations.
TEST(InterpreterTest, PersistentEnvironment) {
    Interpreter interpreter;
    // First, declare a variable.
    interpreter.evaluate("int a = 10;",false);
    // Then, use it in a subsequent expression.
    std::any result = interpreter.evaluate("a + 5;",false);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 15);
}

/*
 * Function Testing
 */
// Test that a simple function with no parameters works correctly.
TEST(InterpreterTest, FunctionCall_NoParameters) {
    // This program declares a function 'foo' that returns 42, then calls foo().
    std::string code = "int foo() { return 42; } foo();";

    Interpreter interpreter;
    std::any result = interpreter.evaluate(code, false);

    // Extract the value and verify it is 42.
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 42);
}

// Test that a function with parameters is declared and called properly.
TEST(InterpreterTest, FunctionCall_WithParameters) {
    // Example: a function 'add' takes two integer parameters and returns their sum.
    std::string code = "int add(int a, int b) { return a + b; } add(10, 32);";

    Interpreter interpreter;
    std::any result = interpreter.evaluate(code, false);

    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 42);
}

// Test that function declarations persist in the environment so subsequent calls work.
TEST(InterpreterTest, FunctionDeclaration_PersistState) {
    // Here we first declare a function, then call it in two separate commands.
    // In an interactive session, the function declaration would persist.
    std::string declCode = "int square(int x) { return x*x; }";
    std::string callCode1 = "square(5);";
    std::string callCode2 = "square(6);";

    Interpreter interpreter;
    // Evaluate the declaration first.
    interpreter.evaluate(declCode, false);

    // Then evaluate two separate calls.
    std::any result1 = interpreter.evaluate(callCode1, false);
    std::any result2 = interpreter.evaluate(callCode2, false);

    int value1 = std::any_cast<int>(result1);
    int value2 = std::any_cast<int>(result2);
    EXPECT_EQ(value1, 25);
    EXPECT_EQ(value2, 36);
}

// (Optional) Test a function that uses a compound statement with local declarations.
TEST(InterpreterTest, FunctionLocalDeclaration) {
    // Define a function that declares a local variable and returns it.
    std::string code = "int localTest() { int a = 10; return a; } localTest();";

    Interpreter interpreter;
    std::any result = interpreter.evaluate(code, false);

    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 10);
}
/*
 * Whole file testing
 */
// Test 1: A simple complete program returning a constant value.
TEST(InterpreterFileModeTest, BasicMainReturn) {
    std::string code = "int main() { return 42; }";
    Interpreter interpreter;
    // Evaluate in file mode (true means file mode)
    std::any result = interpreter.evaluate(code, true);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 42);
}

// Test 2: A program with a function declaration and a call from main.
TEST(InterpreterFileModeTest, FunctionDeclarationAndCall) {
    std::string code =
        "int add(int a, int b) { return a + b; } "
        "int main() { return add(20, 22); }";
    Interpreter interpreter;
    std::any result = interpreter.evaluate(code, true);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 42);
}

// Test 3: Program with multiple global declarations.
// For example, ensure that a program with several statements is processed correctly.
TEST(InterpreterFileModeTest, MultipleGlobalDeclarations) {
    std::string code =
        "int x = 10; "
        "int y = 32; "
        "int main() { return x + y; }";
    Interpreter interpreter;
    std::any result = interpreter.evaluate(code, true);
    int value = std::any_cast<int>(result);
    EXPECT_EQ(value, 42);
}

// Test 4: Check that the file mode starts with a fresh environment.
// (This may require that your interpreter's environment is reset on each call in file mode.)
TEST(InterpreterFileModeTest, FreshEnvironmentForEachRun) {
    // First file run defines a global variable and returns its value.
    std::string code1 = "int main() { int a = 100; return a; }";
    // Second file run does not define 'a', so it should result in an error or default.
    std::string code2 = "int main() { return 0; }";

    Interpreter interpreter;
    std::any result1 = interpreter.evaluate(code1, true);
    int value1 = std::any_cast<int>(result1);
    EXPECT_EQ(value1, 100);

    // Run a second file on a fresh environment.
    Interpreter freshInterpreter;
    std::any result2 = freshInterpreter.evaluate(code2, true);
    int value2 = std::any_cast<int>(result2);
    EXPECT_EQ(value2, 0);
}

