#include "gtest/gtest.h"
#include "antlr4-runtime.h"
#include "CParser.h"
#include "CLexer.h"
#include "CInterpreterVisitor.h"
#include "Environment.h"
#include <sstream>
#include <any>
#include <stdexcept>
#include <typeinfo>

// Helper function to extract a numeric value (as double) from the result.
// It converts int values to double if needed.
double getNumericValue(const std::any &result) {
    if (result.type() == typeid(VarValue)) {
        VarValue num = std::any_cast<VarValue>(result);
        return std::visit([](auto v) -> double { return static_cast<double>(v); }, num);
    } else if (result.type() == typeid(int)) {
        return static_cast<double>(std::any_cast<int>(result));
    } else if (result.type() == typeid(double)) {
        return std::any_cast<double>(result);
    } else {
        throw std::runtime_error("Result is not numeric");
    }
}


// Helper function to evaluate an expression using the visitor.
// Assumes that the top-level rule is EvaluateExpression (i.e. an expression followed by EOF).
std::any evaluateExpression(const std::string &expr) {
    // Create an input stream from the expression string.
    antlr4::ANTLRInputStream inputStream(expr);
    // Create a lexer for the input.
    CLexer lexer(&inputStream);
    // Tokenize the input.
    antlr4::CommonTokenStream tokens(&lexer);
    // Create a parser for the tokens.
    CParser parser(&tokens);
    // Parse using the top-level rule.
    CParser::ReplInputContext* tree = parser.replInput();
    // Create an environment (global scope) for the visitor.
    Environment env;
    // Create your visitor instance.
    CInterpreterVisitor visitor(&env);
    // Visit the parse tree and return the result.
    return visitor.visit(tree);
}

// Test that a number literal is evaluated correctly.
TEST(CInterpreterVisitorTest, NumberLiteral) {
    std::any result = evaluateExpression("42");
    // Since "42" has no decimal point, it should be returned as an int.
    // Our helper converts int to double for comparison.
    double value = getNumericValue(result);
    EXPECT_EQ(value, 42);
}

// Test addition.
TEST(CInterpreterVisitorTest, Addition) {
    std::any result = evaluateExpression("3 + 4");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 7);
}

// Test unary minus.
TEST(CInterpreterVisitorTest, UnaryMinus) {
    std::any result = evaluateExpression("-5");
    double value = getNumericValue(result);
    EXPECT_EQ(value, -5);
}

// Test expression with parentheses.
TEST(CInterpreterVisitorTest, Parentheses) {
    std::any result = evaluateExpression("(3 + 4)");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 7);
}

// Test multiplication combined with addition.
TEST(CInterpreterVisitorTest, MultiplicationAndAddition) {
    std::any result = evaluateExpression("2 * (3 + 4)");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 14);
}

// Test division.
TEST(CInterpreterVisitorTest, Division) {
    std::any result = evaluateExpression("8 / 2");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 4);
}

// Test operator precedence: multiplication before addition.
TEST(CInterpreterVisitorTest, OperatorPrecedence) {
    // Expression "2 + 3 * 4" should yield 14.
    std::any result = evaluateExpression("2 + 3 * 4");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 14);
}

// Test a more complex expression.
TEST(CInterpreterVisitorTest, ComplexExpression) {
    // Expression: "2 * (3 + 4) - 5 / (1 + 1)" -> 2*7 - 5/2 = 14 - 2 = 12
    std::any result = evaluateExpression("2 * (3 + 4) - 5 / (1 + 1)");
    double value = getNumericValue(result);
    EXPECT_DOUBLE_EQ(value, 12);
}



TEST(CInterpreterVisitorTest, CharLiteral) {
    // For testing, we simulate the char literal directly.
    // Let's assume your grammar interprets "'A'" as a char literal.
    // Since 'A' has ASCII code 65, we expect the result to be 65 (as an int).
    // For simplicity, we can evaluate it as an expression if your grammar supports it.
    std::any result = evaluateExpression("'A'");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 65);
}

// Test variable declaration and usage.
TEST(CInterpreterVisitorTest, VariableDeclarationAndReference) {
    // We'll evaluate a sequence of statements: a declaration and then an expression that uses it.
    // For instance, "int a = 10; a + 5" should yield 15.
    std::string code = "int a = 10; a + 5";

    antlr4::ANTLRInputStream inputStream(code);
    CLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CParser parser(&tokens);
    // Use the ExecuteStatements alternative.
    CParser::ReplInputContext* tree = parser.replInput();
    Environment env;
    CInterpreterVisitor visitor(&env);
    std::any result = visitor.visit(tree);
    double value = getNumericValue(result);
    EXPECT_EQ(value, 15);
}
TEST(CInterpreterVisitorTest, DivisionByZero) {
    EXPECT_THROW({
        evaluateExpression("5 / 0");
    }, std::runtime_error);
}
TEST(CInterpreterVisitorTest, InvalidSyntax) {
    EXPECT_THROW({
        evaluateExpression("2 +");
    }, std::runtime_error);
}
TEST(CInterpreterVisitorTest, CompoundStatement) {
    std::string code = "{ int a = 10; a + 5; }";
    antlr4::ANTLRInputStream inputStream(code);
    CLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CParser parser(&tokens);
    CParser::ReplInputContext* tree = parser.replInput();
    Environment env;
    CInterpreterVisitor visitor(&env);
    std::any result = visitor.visit(tree);
    double value = getNumericValue(result);
    EXPECT_EQ(value, 15);
}
TEST(CInterpreterVisitorTest, MixedNegatives) {
    std::any result = evaluateExpression("-3 + 5 - (-2)");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 4); // -3 + 5 + 2 = 4
}

TEST(CInterpreterVisitorTest, CharArithmetic) {
    // For example, if 'A' has ASCII 65, then "10 + 'A'" should yield 75.
    std::any result = evaluateExpression("10 + 'A'");
    double value = getNumericValue(result);
    EXPECT_EQ(value, 75);
}


TEST(CInterpreterVisitorTest, IntegerDivision) {
    // With integer division, "5 / 2" should yield 2.
    std::any result = evaluateExpression("5 / 2");
    // Since both 5 and 2 are ints, we expect an int result.
    // Use std::any_cast<int> directly:
    int value;
    if (result.type() == typeid(VarValue)) {
        VarValue num = std::any_cast<VarValue>(result);
        value = std::visit([](auto v) -> double { return static_cast<double>(v); }, num);
    }
    else {
        value = std::any_cast<int>(result);
    }
    EXPECT_EQ(value, 2);
}
