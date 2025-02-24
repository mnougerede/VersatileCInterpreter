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

// Helper template to extract a value of type T from std::any.
template <typename T>
T getValue(const std::any &result) {
    return std::any_cast<T>(result);
}


// Helper function to evaluate an expression using the visitor.
std::any evaluateExpression(const std::string &expr) {
    antlr4::ANTLRInputStream inputStream(expr);
    CLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CParser parser(&tokens);
    // Use the top-level rule for expressions.
    CParser::ReplInputContext* tree = parser.replInput();
    Environment env;  // Global environment.
    CInterpreterVisitor visitor(&env);
    return visitor.visit(tree);
}

// ----------------------- Numeric and Arithmetic Tests ------------------
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


TEST(CInterpreterVisitorTest, DivisionByZero) {
    EXPECT_THROW({
        evaluateExpression("5 / 0");
    }, std::runtime_error);
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
// ------------------- Relational Operators Tests -------------------

// Test less-than operator (true case).
TEST(CInterpreterVisitorTest, LessThanTrue) {
    std::any result = evaluateExpression("3 < 5");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test less-than operator (false case).
TEST(CInterpreterVisitorTest, LessThanFalse) {
    std::any result = evaluateExpression("5 < 3");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test less-than-or-equal operator.
TEST(CInterpreterVisitorTest, LessThanOrEqual) {
    std::any result = evaluateExpression("3 <= 3");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test greater-than operator.
TEST(CInterpreterVisitorTest, GreaterThan) {
    std::any result = evaluateExpression("5 > 3");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test greater-than-or-equal operator.
TEST(CInterpreterVisitorTest, GreaterThanOrEqual) {
    std::any result = evaluateExpression("3 >= 3");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// ------------------- Equality Operators Tests -------------------

// Test equality operator (true case).
TEST(CInterpreterVisitorTest, EqualityTrue) {
    std::any result = evaluateExpression("3 == 3");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test equality operator (false case).
TEST(CInterpreterVisitorTest, EqualityFalse) {
    std::any result = evaluateExpression("3 == 4");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test not-equals operator (true case).
TEST(CInterpreterVisitorTest, NotEqualsTrue) {
    std::any result = evaluateExpression("3 != 4");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test not-equals operator (false case).
TEST(CInterpreterVisitorTest, NotEqualsFalse) {
    std::any result = evaluateExpression("3 != 3");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 0);
}

// ------------------- Logical Operators Tests -------------------

// Test logical AND (true case).
TEST(CInterpreterVisitorTest, LogicalAndTrue) {
    std::any result = evaluateExpression("1 && 1");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test logical AND (false case).
TEST(CInterpreterVisitorTest, LogicalAndFalse) {
    std::any result = evaluateExpression("1 && 0");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test logical OR (true case).
TEST(CInterpreterVisitorTest, LogicalOrTrue) {
    std::any result = evaluateExpression("0 || 1");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test logical OR (false case).
TEST(CInterpreterVisitorTest, LogicalOrFalse) {
    std::any result = evaluateExpression("0 || 0");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test logical NOT (true case).
TEST(CInterpreterVisitorTest, LogicalNotTrue) {
    std::any result = evaluateExpression("!0");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test logical NOT (false case).
TEST(CInterpreterVisitorTest, LogicalNotFalse) {
    std::any result = evaluateExpression("!1");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 0);
}

// ------------------- Compound Logical/Relational Tests -------------------

// Test a compound logical expression.
TEST(CInterpreterVisitorTest, CompoundLogicalExpression1) {
    // Expression: "(3 < 5) && (3 == 3)" should yield true.
    std::any result = evaluateExpression("(3 < 5) && (3 == 3)");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test another compound logical expression.
TEST(CInterpreterVisitorTest, CompoundLogicalExpression2) {
    // Expression: "(3 > 5) || (2 < 4)" should yield true.
    std::any result = evaluateExpression("(3 > 5) || (2 < 4)");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test a mixed compound expression.
TEST(CInterpreterVisitorTest, ComplexExpressionMixed) {
    // For example, "((2+3)*2) >= (10-1)" should yield true (1) because 10 >= 9.
    std::any result = evaluateExpression("((2+3)*2) >= (10-1)");
    int value = getValue<int>(result);
    EXPECT_EQ(value, 1);
}