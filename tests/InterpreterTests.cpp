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
