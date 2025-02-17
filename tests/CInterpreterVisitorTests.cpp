#include "gtest/gtest.h"
#include "antlr4-runtime.h"
#include "CParser.h"
#include "CLexer.h"
#include "CInterpreterVisitor.h"
#include <sstream>
#include <any>

// Helper function to evaluate an expression using the visitor.
std::any evaluateExpression(const std::string &expr) {
    // Create an input stream from the expression string.
    antlr4::ANTLRInputStream inputStream(expr);
    // Create a lexer for the input.
    CLexer lexer(&inputStream);
    // Tokenize the input.
    antlr4::CommonTokenStream tokens(&lexer);
    // Create a parser for the tokens.
    CParser parser(&tokens);
    // Parse using the top-level rule (assumes replInput accepts a bare expression).
    CParser::ReplInputContext* tree = parser.replInput();
    // Create your visitor instance.
    CInterpreterVisitor visitor;
    // Visit the parse tree and return the result.
    return visitor.visit(tree);
}

// Test that a number literal is evaluated correctly.
TEST(CInterpreterVisitorTest, NumberLiteral) {
    std::any result = evaluateExpression("42");
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, 42);
}

// Test addition.
TEST(CInterpreterVisitorTest, Addition) {
    std::any result = evaluateExpression("3 + 4");
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, 7);
}

// Test unary minus.
TEST(CInterpreterVisitorTest, UnaryMinus) {
    std::any result = evaluateExpression("-5");
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, -5);
}

// Test expression with parentheses.
TEST(CInterpreterVisitorTest, Parentheses) {
    std::any result = evaluateExpression("(3 + 4)");
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, 7);
}

// Test multiplication combined with addition.
TEST(CInterpreterVisitorTest, MultiplicationAndAddition) {
    std::any result = evaluateExpression("2 * (3 + 4)");
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, 14);
}

// Test division.
TEST(CInterpreterVisitorTest, Division) {
    std::any result = evaluateExpression("8 / 2");
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, 4);
}

// Test operator precedence: multiplication before addition.
TEST(CInterpreterVisitorTest, OperatorPrecedence) {
    // Expression "2 + 3 * 4" should yield 14.
    std::any result = evaluateExpression("2 + 3 * 4");
    double value = std::any_cast<double>(result);
    EXPECT_EQ(value, 14);
}

// Test a more complex expression.
TEST(CInterpreterVisitorTest, ComplexExpression) {
    // Expression: "2 * (3 + 4) - 5 / (1 + 1)" -> 2*7 - 5/2 = 14 - 2.5 = 11.5
    std::any result = evaluateExpression("2 * (3 + 4) - 5 / (1 + 1)");
    double value = std::any_cast<double>(result);
    EXPECT_DOUBLE_EQ(value, 11.5);
}
