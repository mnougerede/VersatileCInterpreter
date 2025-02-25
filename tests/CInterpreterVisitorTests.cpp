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

// Helper template to extract a value of type T from a VarValue stored in std::any.
template <typename T>
T extractValue(const std::any &result) {
    // We assume the result holds a VarValue.
    VarValue value = std::any_cast<VarValue>(result);
    return std::visit([](auto v) -> T { return static_cast<T>(v); }, value);
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
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 42);
}

// Test addition.
TEST(CInterpreterVisitorTest, Addition) {
    std::any result = evaluateExpression("3 + 4");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 7);
}

// Test unary minus.
TEST(CInterpreterVisitorTest, UnaryMinus) {
    std::any result = evaluateExpression("-5");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, -5);
}

// Test expression with parentheses.
TEST(CInterpreterVisitorTest, Parentheses) {
    std::any result = evaluateExpression("(3 + 4)");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 7);
}

// Test multiplication combined with addition.
TEST(CInterpreterVisitorTest, MultiplicationAndAddition) {
    std::any result = evaluateExpression("2 * (3 + 4)");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 14);
}

// Test division.
TEST(CInterpreterVisitorTest, Division) {
    std::any result = evaluateExpression("8 / 2");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 4);
}

// Test operator precedence: multiplication before addition.
TEST(CInterpreterVisitorTest, OperatorPrecedence) {
    // Expression "2 + 3 * 4" should yield 14.
    std::any result = evaluateExpression("2 + 3 * 4");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 14);
}

// Test a more complex expression.
TEST(CInterpreterVisitorTest, ComplexExpression) {
    // Expression: "2 * (3 + 4) - 5 / (1 + 1)" -> 2*7 - 5/2 = 14 - 2 = 12
    std::any result = evaluateExpression("2 * (3 + 4) - 5 / (1 + 1)");
    int value = extractValue<int>(result);
    EXPECT_DOUBLE_EQ(value, 12);
}



TEST(CInterpreterVisitorTest, CharLiteral) {
    // For testing, we simulate the char literal directly.
    // Let's assume your grammar interprets "'A'" as a char literal.
    // Since 'A' has ASCII code 65, we expect the result to be 65 (as an int).
    // For simplicity, we can evaluate it as an expression if your grammar supports it.
    std::any result = evaluateExpression("'A'");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 65);
}
// Test floating-point arithmetic.
TEST(CInterpreterVisitorTest, FloatingPointArithmetic) {
    std::any result = evaluateExpression("3.5 + 2.5");
    double value = extractValue<double>(result);
    EXPECT_DOUBLE_EQ(value, 6.0);
}

TEST(CInterpreterVisitorTest, DivisionByZero) {
    EXPECT_THROW({
        evaluateExpression("5 / 0");
    }, std::runtime_error);
}


TEST(CInterpreterVisitorTest, MixedNegatives) {
    std::any result = evaluateExpression("-3 + 5 - (-2)");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 4); // -3 + 5 + 2 = 4
}

TEST(CInterpreterVisitorTest, CharArithmetic) {
    // For example, if 'A' has ASCII 65, then "10 + 'A'" should yield 75.
    std::any result = evaluateExpression("10 + 'A'");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 75);
}


// Test integer division explicitly.
TEST(CInterpreterVisitorTest, IntegerDivision) {
    std::any result = evaluateExpression("5 / 2");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 2);
}

// Test floating-point division when one operand is floating point.
TEST(CInterpreterVisitorTest, FloatingPointDivision) {
    std::any result = evaluateExpression("5 / 2.0");
    double value = extractValue<double>(result);
    EXPECT_DOUBLE_EQ(value, 2.5);
}

// ------------------- Relational Operators Tests -------------------

// Test less-than operator (true case).
TEST(CInterpreterVisitorTest, LessThanTrue) {
    std::any result = evaluateExpression("3 < 5");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test less-than operator (false case).
TEST(CInterpreterVisitorTest, LessThanFalse) {
    std::any result = evaluateExpression("5 < 3");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test less-than-or-equal operator.
TEST(CInterpreterVisitorTest, LessThanOrEqual) {
    std::any result = evaluateExpression("3 <= 3");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test greater-than operator.
TEST(CInterpreterVisitorTest, GreaterThan) {
    std::any result = evaluateExpression("5 > 3");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test greater-than-or-equal operator.
TEST(CInterpreterVisitorTest, GreaterThanOrEqual) {
    std::any result = evaluateExpression("3 >= 3");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// ------------------- Equality Operators Tests -------------------

// Test equality operator (true case).
TEST(CInterpreterVisitorTest, EqualityTrue) {
    std::any result = evaluateExpression("3 == 3");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test equality operator (false case).
TEST(CInterpreterVisitorTest, EqualityFalse) {
    std::any result = evaluateExpression("3 == 4");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test not-equals operator (true case).
TEST(CInterpreterVisitorTest, NotEqualsTrue) {
    std::any result = evaluateExpression("3 != 4");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test not-equals operator (false case).
TEST(CInterpreterVisitorTest, NotEqualsFalse) {
    std::any result = evaluateExpression("3 != 3");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}

// ------------------- Logical Operators Tests -------------------

// Test logical AND (true case).
TEST(CInterpreterVisitorTest, LogicalAndTrue) {
    std::any result = evaluateExpression("1 && 1");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test logical AND (false case).
TEST(CInterpreterVisitorTest, LogicalAndFalse) {
    std::any result = evaluateExpression("1 && 0");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test logical OR (true case).
TEST(CInterpreterVisitorTest, LogicalOrTrue) {
    std::any result = evaluateExpression("0 || 1");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test logical OR (false case).
TEST(CInterpreterVisitorTest, LogicalOrFalse) {
    std::any result = evaluateExpression("0 || 0");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}

// Test logical NOT (true case).
TEST(CInterpreterVisitorTest, LogicalNotTrue) {
    std::any result = evaluateExpression("!0");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test logical NOT (false case).
TEST(CInterpreterVisitorTest, LogicalNotFalse) {
    std::any result = evaluateExpression("!1");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}

// ------------------- Compound Logical/Relational Tests -------------------

// Test a compound logical expression.
TEST(CInterpreterVisitorTest, CompoundLogicalExpression1) {
    // Expression: "(3 < 5) && (3 == 3)" should yield true.
    std::any result = evaluateExpression("(3 < 5) && (3 == 3)");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test another compound logical expression.
TEST(CInterpreterVisitorTest, CompoundLogicalExpression2) {
    // Expression: "(3 > 5) || (2 < 4)" should yield true.
    std::any result = evaluateExpression("(3 > 5) || (2 < 4)");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

// Test a mixed compound expression.
TEST(CInterpreterVisitorTest, ComplexExpressionMixed) {
    // For example, "((2+3)*2) >= (10-1)" should yield true (1) because 10 >= 9.
    std::any result = evaluateExpression("((2+3)*2) >= (10-1)");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}
// --------------------- Assignment Tests -----------------

// Test that an assignment expression returns the assigned value.
// For example, "int a; a = 7" should yield 7.
TEST(CInterpreterVisitorTest, AssignmentExpressionReturnsValue) {
    std::any result = evaluateExpression("int a; a = 7;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 7);
}

// Test that a variable declared with an assignment is stored correctly.
// For example, "int a = 10; a" should yield 10.
TEST(CInterpreterVisitorTest, AssignmentDuringDeclaration) {
    std::any result = evaluateExpression("int a = 10; a;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 10);
}

// Test updating a variable with a new int value.
// For example, "int a = 10; a = 5; a" should yield 5.
TEST(CInterpreterVisitorTest, UpdateIntValue) {
    std::any result = evaluateExpression("int a = 10; a = 5; a;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 5);
}

// Test assigning a char literal to an int variable.
// In C, a char is implicitly converted to its ASCII value.
TEST(CInterpreterVisitorTest, AssignCharToInt) {
    // 'A' has ASCII code 65.
    std::any result = evaluateExpression("int a = 'A'; a;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 65);
}

// Test assigning a double to an int variable (which should truncate).
TEST(CInterpreterVisitorTest, AssignDoubleToInt) {
    // In standard C, assigning 3.9 to an int yields 3.
    std::any result = evaluateExpression("int a = 10; a = 3.9; a;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 3);
}

// Test that the assignment expression itself evaluates to the new value.
// For example, "int a = 5; (a = 42)" should return 42.
TEST(CInterpreterVisitorTest, AssignmentExpressionValue) {
    std::any result = evaluateExpression("int a = 5; (a = 42);");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 42);
}

// Test that multiple assignments in sequence yield the value of the last assignment.
// For example, "int a = 5; a = 7; a = 9; a" should yield 9.
TEST(CInterpreterVisitorTest, MultipleAssignments) {
    std::any result = evaluateExpression("int a = 5; a = 7; a = 9; a;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 9);
}