#include "gtest/gtest.h"
#include "Interpreter.h"
#include <any>
#include <stdexcept>

TEST(InterpreterTest, SimpleExpression) {
    Interpreter interpreter;
    // Given an expression "3 + 4;", we expect the result to be 7.
    std::any result = interpreter.evaluate("3 + 4");
    auto value = std::any_cast<double>(result);
    EXPECT_EQ(value, 7);
}

TEST(InterpreterTest, ComplexExpression) {
    Interpreter interpreter;
    // Evaluate a more complex expression: 2 * (3 + 5) - 4 = 12.
    std::any result = interpreter.evaluate("2 * (3 + 5) - 4");
    auto value = std::any_cast<double>(result);
    EXPECT_EQ(value, 12);
}

TEST(InterpreterTest, InvalidExpressionThrows) {
    Interpreter interpreter;
    // An invalid expression (e.g., missing operand) should throw an exception.
    EXPECT_THROW({
        interpreter.evaluate("3 + ");
    }, std::exception);
}
// Test: Negative numbers should be handled correctly.
TEST(InterpreterTest, NegativeNumbers) {
    Interpreter interpreter;
    // Expression: -5 + 2 should yield -3.
    std::any result = interpreter.evaluate("-5 + 2");
    auto value = std::any_cast<double>(result);
    EXPECT_EQ(value, -3);
}

// Test: Operator precedence (multiplication before addition).
TEST(InterpreterTest, OperatorPrecedence) {
    Interpreter interpreter;
    // Expression: 3 + 2 * 4 should yield 11 (not 20).
    std::any result = interpreter.evaluate("3 + 2 * 4");
    auto value = std::any_cast<double>(result);
    EXPECT_EQ(value, 11);
}

// Test: Parentheses overriding precedence.
TEST(InterpreterTest, Parentheses) {
    Interpreter interpreter;
    // Expression: (3 + 2) * 4 should yield 20.
    std::any result = interpreter.evaluate("(3 + 2) * 4");
    auto value = std::any_cast<double>(result);
    EXPECT_EQ(value, 20);
}

// Test: Floating-point arithmetic.
TEST(InterpreterTest, FloatingPointArithmetic) {
    Interpreter interpreter;
    // Expression: 3.5 + 2.5 should yield 6.0.
    std::any result = interpreter.evaluate("3.5 + 2.5");
    auto value = std::any_cast<double>(result);
    EXPECT_EQ(value, 6.0);
}

// Test: Handling extra whitespace.
TEST(InterpreterTest, ExtraWhitespace) {
    Interpreter interpreter;
    // Expression with extra spaces: "    3   +    4 " should yield 7.
    std::any result = interpreter.evaluate("    3   +    4 ");
    auto value = std::any_cast<double>(result);
    EXPECT_EQ(value, 7);
}

// Test: Unmatched parentheses should throw an exception.
TEST(InterpreterTest, UnmatchedParenthesesThrows) {
    Interpreter interpreter;
    // Expression: "3 + (4 - 1" is missing a closing parenthesis.
    EXPECT_THROW({
        interpreter.evaluate("3 + (4 - 1");
    }, std::exception);
}

// Test: Division by zero (if your interpreter is designed to throw).
TEST(InterpreterTest, DivisionByZeroThrows) {
    Interpreter interpreter;
    // Expression: "5 / 0" should throw an exception.
    EXPECT_THROW({
        interpreter.evaluate("5 / 0");
    }, std::exception);
}