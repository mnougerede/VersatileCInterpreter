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

// removed basic logical tests as they always pass (return last leaf)

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
// ---------- Logical OR and AND with non-Boolean operands ----------

TEST(CInterpreterVisitorTest, LogicalOrNonBooleanOperands) {
    // "2 || 2" should yield 1 even though both operands are 2 (nonzero).
    std::any result = evaluateExpression("2 || 2");
    int value = extractValue<int>(result);
    // If the visitor just returns the last child, it might return 2.
    // So we expect 1 (true) if logical OR is properly implemented.
    EXPECT_EQ(value, 1);
}

TEST(CInterpreterVisitorTest, LogicalAndNonBooleanOperands) {
    // "2 && 2" should yield 1 (true) when both operands are nonzero.
    std::any result = evaluateExpression("2 && 2");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

TEST(CInterpreterVisitorTest, LogicalOrMixedOperands) {
    // "2 || 0" should yield 1 because at least one operand is true.
    std::any result = evaluateExpression("2 || 0");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
}

TEST(CInterpreterVisitorTest, LogicalAndMixedOperands) {
    // "0 && 2" should yield 0 because one operand is false.
    std::any result = evaluateExpression("0 && 2");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}
TEST(CInterpreterVisitorTest, LogicalNotNonBoolean) {
    // "!2" should yield 0 because 2 is nonzero (true), so its logical NOT is false (0).
    std::any result = evaluateExpression("!2");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 0);
}
TEST(CInterpreterVisitorTest, ComplexLogicalExpression) {
    // Example: "((3 < 5) && (4 == 4)) || (2 > 3)"
    // (3 < 5) is true (1) and (4 == 4) is true (1), so (1 && 1) is 1;
    // (2 > 3) is false (0), so 1 || 0 should yield 1.
    std::any result = evaluateExpression("((3 < 5) && (4 == 4)) || (2 > 3)");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 1);
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
// -------------------- If-Then-Else Expression Tests --------------------

// Test an if-then-else expression where the condition is true.
TEST(CInterpreterVisitorTest, IfThenElseTrue) {
    // Expression: if (1) 42; else 0; should yield 42.
    std::any result = evaluateExpression("if (1) 42; else 0;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 42);
}

// Test an if-then-else expression where the condition is false.
TEST(CInterpreterVisitorTest, IfThenElseFalse) {
    // Expression: if (0) 42; else 10; should yield 10.
    std::any result = evaluateExpression("if (0) 42; else 10;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 10);
}

// Test an if statement without an else branch when the condition is true.
TEST(CInterpreterVisitorTest, IfThenNoElseTrue) {
    // Expression: if (1) 10; should yield 10.
    std::any result = evaluateExpression("if (1) 10;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 10);
}

// Test an if statement without an else branch when the condition is false.
TEST(CInterpreterVisitorTest, IfThenNoElseFalse) {
    std::any result = evaluateExpression("if (0) 10;");

    // Expect that no branch was executed, and thus the result is empty.
    EXPECT_FALSE(result.has_value());
}


// Test nested if-then-else expressions.
TEST(CInterpreterVisitorTest, NestedIfThenElse) {
    // Expression: if (1) { if (0) 5; else 7; } else 0; should yield 7.
    std::any result = evaluateExpression("if (1) { if (0) 5; else 7; } else 0;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 7);
}

// Test a compound if condition.
TEST(CInterpreterVisitorTest, ComplexIfCondition) {
    // Expression: if ((2 + 3) == 5) 100; else 200; should yield 100.
    std::any result = evaluateExpression("if ((2 + 3) == 5) 100; else 200;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 100);
}

// -------------------- Iteration Tests --------------------
// Test a while loop that increments a variable from 0 to 5.
TEST(CInterpreterVisitorTest, WhileLoopTest) {
    // The code declares 'i', then runs a while loop until i is 5, then returns i.
    std::any result = evaluateExpression("int i = 0; while(i < 5) { i = i + 1; } i;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 5);
}

// Test a do-while loop that increments a variable from 0 to 5.
TEST(CInterpreterVisitorTest, DoWhileLoopTest) {
    // The code declares 'i', then runs a do-while loop until i is 5, then returns i.
    std::any result = evaluateExpression("int i = 0; do { i = i + 1; } while(i < 5); i;");
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 5);
}

// Test a for loop with a declaration in the initializer that calculates a sum.
TEST(CInterpreterVisitorTest, ForLoopWithDeclarationTest) {
    // The code declares 'sum' and a loop variable 'i' in the for loop.
    // The loop adds i to sum for i = 0 to 4, then returns sum.
    std::any result = evaluateExpression(
        "int sum = 0; "
        "for (int i = 0; i < 5; i = i + 1) { "
        "    sum = sum + i; "
        "} "
        "sum;"
    );
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 10);  // 0 + 1 + 2 + 3 + 4 = 10
}

// Test a for loop with an expression initializer that increments a variable.
TEST(CInterpreterVisitorTest, ForLoopWithExpressionInitializerTest) {
    // Here, 'i' is declared before the loop and then set via the for loop's initializer.
    std::any result = evaluateExpression(
        "int i = 0; "
        "for (i = 0; i < 5; i = i + 1) { "
        "    /* empty body */ "
        "} "
        "i;"
    );
    int value = extractValue<int>(result);
    EXPECT_EQ(value, 5);
}
