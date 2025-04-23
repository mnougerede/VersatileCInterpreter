#include "gtest/gtest.h"
#include "antlr4-runtime.h"
#include "CParser.h"
#include "CLexer.h"
#include "CInterpreterVisitor.h"
#include "Environment.h"
#include <any>
#include <stdexcept>
#include <string>
#include <type_traits>

// Helper to extract a primitive T from the VarValue wrapped in std::any
template<typename T>
T extractValue(const std::any &wrapped) {
    auto vv = std::any_cast<VarValue>(wrapped);
    return std::visit([](auto v) -> T { return static_cast<T>(v); }, vv);
}

// Evaluate any mix of declarations & statements using the REPL entry point.
static std::any evalRepl(const std::string &src) {
    antlr4::ANTLRInputStream       input(src);
    CLexer                         lexer(&input);
    antlr4::CommonTokenStream      tokens(&lexer);
    CParser                        parser(&tokens);
    auto *ctx = parser.replInput();           // allows declaration | statement
    Environment                    env;
    CInterpreterVisitor            vis(&env, &tokens);
    return vis.visit(ctx);
}

// ---------------- Numeric & Arithmetic ----------------

TEST(VisitorREPL, NumberLiteral) {
    EXPECT_EQ(extractValue<int>(evalRepl("42;")), 42);
}

TEST(VisitorREPL, Addition) {
    EXPECT_EQ(extractValue<int>(evalRepl("3 + 4;")), 7);
}

TEST(VisitorREPL, UnaryMinus) {
    EXPECT_EQ(extractValue<int>(evalRepl("-5;")), -5);
}

TEST(VisitorREPL, Parentheses) {
    EXPECT_EQ(extractValue<int>(evalRepl("(3 + 4);")), 7);
}

TEST(VisitorREPL, MulAddPrecedence) {
    EXPECT_EQ(extractValue<int>(evalRepl("2 + 3 * 4;")), 14);
}

TEST(VisitorREPL, ComplexArithmetic) {
    // 2*(3+4) - 5/(1+1) = 14 - 2 = 12
    EXPECT_EQ(extractValue<int>(evalRepl("2 * (3 + 4) - 5 / (1 + 1);")), 12);
}

TEST(VisitorREPL, DivisionByZero) {
    EXPECT_THROW(evalRepl("5/0;"), std::runtime_error);
}

// ------------- Floating Point & Char -------------

TEST(VisitorREPL, FloatingPointArithmetic) {
    EXPECT_DOUBLE_EQ(extractValue<double>(evalRepl("3.5 + 2.5;")), 6.0);
}

TEST(VisitorREPL, CharLiteral) {
    // 'A' → 65
    EXPECT_EQ(extractValue<int>(evalRepl("'A';")), 65);
}

TEST(VisitorREPL, CharArithmetic) {
    // 10 + 'A' → 10 + 65 = 75
    EXPECT_EQ(extractValue<int>(evalRepl("10 + 'A';")), 75);
}

// ---------------- Relational & Equality ----------------

TEST(VisitorREPL, LessThanTrue)  { EXPECT_EQ(extractValue<int>(evalRepl("3 < 5;")), 1); }
TEST(VisitorREPL, LessThanFalse) { EXPECT_EQ(extractValue<int>(evalRepl("5 < 3;")), 0); }
TEST(VisitorREPL, LEQ)           { EXPECT_EQ(extractValue<int>(evalRepl("3 <= 3;")), 1); }
TEST(VisitorREPL, GreaterThan)   { EXPECT_EQ(extractValue<int>(evalRepl("5 > 3;")), 1); }
TEST(VisitorREPL, GEQ)           { EXPECT_EQ(extractValue<int>(evalRepl("3 >= 3;")), 1); }

TEST(VisitorREPL, EqualityTrue)  { EXPECT_EQ(extractValue<int>(evalRepl("3 == 3;")), 1); }
TEST(VisitorREPL, EqualityFalse) { EXPECT_EQ(extractValue<int>(evalRepl("3 == 4;")), 0); }
TEST(VisitorREPL, NotEqTrue)     { EXPECT_EQ(extractValue<int>(evalRepl("3 != 4;")), 1); }
TEST(VisitorREPL, NotEqFalse)    { EXPECT_EQ(extractValue<int>(evalRepl("3 != 3;")), 0); }

// ---------------- Logical ----------------

TEST(VisitorREPL, LogicalNotTrue)   { EXPECT_EQ(extractValue<int>(evalRepl("!0;")), 1); }
TEST(VisitorREPL, LogicalNotFalse)  { EXPECT_EQ(extractValue<int>(evalRepl("!1;")), 0); }
TEST(VisitorREPL, LogicalOr)        { EXPECT_EQ(extractValue<int>(evalRepl("2 || 0;")), 1); }
TEST(VisitorREPL, LogicalAnd)       { EXPECT_EQ(extractValue<int>(evalRepl("2 && 2;")), 1); }
TEST(VisitorREPL, ComplexLogical)   {
    // (3<5 && 4==4) || (2>3) → (1 && 1) || 0 → 1
    EXPECT_EQ(extractValue<int>(evalRepl("((3 < 5) && (4 == 4)) || (2 > 3);")), 1);
}

// ---------------- Assignment & Variables ----------------

TEST(VisitorREPL, DeclAndAssign) {
    EXPECT_EQ(extractValue<int>(evalRepl("int a; a = 7; a;")), 7);
}

TEST(VisitorREPL, InitDuringDecl) {
    EXPECT_EQ(extractValue<int>(evalRepl("int a = 10; a;")), 10);
}

TEST(VisitorREPL, ReassignTruncate) {
    // assign double to int
    EXPECT_EQ(extractValue<int>(evalRepl("int a = 0; a = 3.9; a;")), 3);
}

// ---------------- If/Else ----------------

TEST(VisitorREPL, IfThenElseTrue)  { EXPECT_EQ(extractValue<int>(evalRepl("if(1) 42; else 0;")), 42); }
TEST(VisitorREPL, IfThenElseFalse) { EXPECT_EQ(extractValue<int>(evalRepl("if(0) 1; else 2;")), 2); }
TEST(VisitorREPL, IfNoElseFalse)   { EXPECT_FALSE(evalRepl("if(0) 5;").has_value()); }

// ---------------- Loops ----------------

TEST(VisitorREPL, WhileLoop) {
    EXPECT_EQ(extractValue<int>(evalRepl("int i=0; while(i<3){ i=i+1; } i;")), 3);
}

TEST(VisitorREPL, DoWhileLoop) {
    EXPECT_EQ(extractValue<int>(evalRepl("int i=0; do { i=i+1; } while(i<2); i;")), 2);
}

TEST(VisitorREPL, ForLoopDeclScope) {
    // 'i' declared in for-header is block-local → undefined afterward
    EXPECT_THROW(evalRepl("for(int i=0; i<1; i=i+1){} i;"), std::runtime_error);
}

TEST(VisitorREPL, ForLoopReuseOuter) {
    EXPECT_EQ(extractValue<int>(
        evalRepl("int i=0; for(i=0; i<4; i=i+1){} i;")
    ), 4);
}

// ---------------- Function Calls ----------------

TEST(VisitorREPL, SimpleFunction) {
    auto v = evalRepl(
        "int id(int x) { return x; }  "
        "id(5);"
    );
    EXPECT_EQ(extractValue<int>(v), 5);
}

// ---------------- Scoping Edge-Cases ----------------

TEST(VisitorREPL, Shadowing) {
    EXPECT_EQ(extractValue<int>(
      evalRepl("int x=1; { int x=2; } x;")
    ), 1);
}

TEST(VisitorREPL, BlockLocal) {
    EXPECT_THROW(evalRepl("{ int a=5; } a;"), std::runtime_error);
}

TEST(VisitorREPL, NestedShadow) {
    EXPECT_EQ(extractValue<int>(
      evalRepl("int x=1; { int x=2; { int x=3; } x; }")
    ), 2);
}

// ----------------------------------------------------------------------------
// |                  Function‐call Arity & Typing Tests                      |
// ----------------------------------------------------------------------------

// 1) Calling an undefined function should throw
TEST(VisitorREPL, UndefinedFunctionCall) {
    EXPECT_THROW(
      evalRepl("foo(1);"),
      std::runtime_error
    );
}

// 2) Too few arguments
TEST(VisitorREPL, TooFewArguments) {
    EXPECT_THROW(
      evalRepl("int f(int x, int y) { return x + y; } f(42);"),
      std::runtime_error
    );
}

// 3) Too many arguments
TEST(VisitorREPL, TooManyArguments) {
    EXPECT_THROW(
      evalRepl("int f(int x) { return x; } f(1, 2);"),
      std::runtime_error
    );
}

// 4) int parameter conversion: double → int (truncated)
TEST(VisitorREPL, IntParamConversion) {
    auto v = evalRepl("int f(int x) { return x; } f(3.9);");
    EXPECT_EQ(extractValue<int>(v), 3);
}

// 5) double parameter conversion: int → double
TEST(VisitorREPL, DoubleParamConversion) {
    auto v = evalRepl("double f(double x) { return x; } f(7);");
    EXPECT_DOUBLE_EQ(extractValue<double>(v), 7.0);
}

// 6) return‐type conversion: double → int (truncated)
TEST(VisitorREPL, ReturnTypeConversion) {
    auto v = evalRepl("int f(double x) { return x + 1.2; } f(2.8);");
    EXPECT_EQ(extractValue<int>(v), 4);
}

// 7) char parameter conversion: int → char
TEST(VisitorREPL, CharParamConversion) {
    auto v = evalRepl("char f(char c) { return c; } f(65);");
    // 65 → 'A'
    EXPECT_EQ(extractValue<char>(v), 65);
}

// 8) multi‐parameter happy path
TEST(VisitorREPL, MultiParamFunctionCall) {
    auto v = evalRepl(R"(
    int add(int a, int b) {
      return a + b;
    }
    add(10, 32);
  )");
    EXPECT_EQ(extractValue<int>(v), 42);
}

// other gotchas

TEST(VisitorREPL, DefaultInitializedToZero) {
    EXPECT_EQ(extractValue<int>(evalRepl("int a; a;")), 0);
}


TEST(VisitorREPL, RecursiveFactorial) {
    auto src = R"(
    int fact(int n) {
      if (n <= 1) return 1;
      return n * fact(n-1);
    }
    fact(5);
  )";
    EXPECT_EQ(extractValue<int>(evalRepl(src)), 120);
}
