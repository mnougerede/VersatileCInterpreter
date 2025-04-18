#include "gtest/gtest.h"
#include "REPL.h"
#include <sstream>
#include <string>

TEST(REPLTest, RunValidInput) {
    // Simulate input: a valid expression followed by "exit".
    std::istringstream input("3 + 4;\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    EXPECT_EQ(outStr, "7");

}

TEST(REPLTest, RunInvalidInput) {
    // Simulate input with an invalid expression.
    std::istringstream input("3 + ;\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output,true);

    std::string outStr = output.str();
    // Verify that an error message is printed.
    EXPECT_NE(outStr.find("Error:"), std::string::npos);
}
// Test that the REPL stops on "quit" as well as "exit".
TEST(REPLTest, QuitTerminatesSession) {
    std::istringstream input("5 * 2;\nquit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output, true);

    // The result should be 10.
    std::string outStr = output.str();
    EXPECT_EQ(outStr, "10");
}
// Test that empty input is handled (i.e. ignored) and does not output a result.
TEST(REPLTest, HandlesEmptyInput) {
    // Provide an empty line between valid inputs.
    std::istringstream input("3 * 3;\n\n4 + 1;\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output, true);

    // Expect the first valid result "9" followed immediately by the second valid result "5".
    // In test mode, empty input is skipped.
    std::string outStr = output.str();
    // We check that both "9" and "5" appear in order.
    size_t pos9 = outStr.find("9");
    size_t pos5 = outStr.find("5", pos9);
    EXPECT_NE(pos9, std::string::npos);
    EXPECT_NE(pos5, std::string::npos);
    EXPECT_LT(pos9, pos5);
}

// Test multiple sequential expressions in one session.
TEST(REPLTest, MultipleExpressions) {
    // The input simulates entering three expressions in one session.
    std::istringstream input("2 + 2;\n7 - 3;\n10 / 2;\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // We expect the results concatenated in order: "4", "4", "5"
    // The test mode is set up to not have new lines, so results will be concatenated.
    EXPECT_NE(outStr.find("4"), std::string::npos);
    EXPECT_NE(outStr.find("5"), std::string::npos);

}

TEST(REPLTest, VariableDeclarationAndUsage) {
    // Declare a variable x and then use it.
    std::istringstream input("int x = 10;\nx + 5;\nexit\n");
    std::ostringstream output;
    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // Expect that the expression "x + 5;" evaluates to "15"
    EXPECT_NE(outStr.find("15"), std::string::npos);
}
TEST(REPLTest, FunctionDeclarationAndCall) {
    // In the REPL, a function is defined then used.
    std::istringstream input("int add(int a, int b) { return a + b; } \nadd(7, 8);\nexit\n");
    std::ostringstream output;
    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // Expect that the function call returns 15.
    EXPECT_NE(outStr.find("15"), std::string::npos);
}
TEST(REPLTest, IfElseStatement) {
    // A simple if-else in REPL mode.
    std::istringstream input("if (1) 100; else 200;\nexit\n");
    std::ostringstream output;
    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // In this case, the if-condition is true so the expression should evaluate to 100.
    EXPECT_NE(outStr.find("100"), std::string::npos);
}
TEST(REPLTest, WhileLoop) {
    // Compute a sum using a while loop.
    std::istringstream input("int sum = 0;\nint i = 0;\nwhile (i < 5) { sum = sum + i; i = i + 1; } \nsum;\nexit\n");
    std::ostringstream output;
    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // For i=0..4, sum should be 0+1+2+3+4 = 10.
    EXPECT_NE(outStr.find("10"), std::string::npos);
}
TEST(REPLTest, NestedScopes) {
    // Try to declare a variable in a block and then reference it outside.
    // This should either return an error
    std::istringstream input("{ int a = 5; }\na;\nexit\n");
    std::ostringstream output;
    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // We expect an error message (or a blank output) when referencing 'a' outside its scope.
    // Here we check that the output contains "Error:".
    EXPECT_NE(outStr.find("Error:"), std::string::npos);
}
TEST(REPLTest, FunctionReturn) {
    // Define and call a function that uses a return statement.
    std::istringstream input("int f() { return 99; } \nf();\nexit\n");
    std::ostringstream output;
    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // Expect the function call to return 99.
    EXPECT_NE(outStr.find("99"), std::string::npos);
}
TEST(REPLTest, MultipleSequentialCommands) {
    std::istringstream input("2 + 2;\n3 * 3;\n7 - 1;\nexit\n");
    std::ostringstream output;
    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // Expect "4", "9", and "6" to appear in order.
    size_t pos4 = outStr.find("4");
    size_t pos9 = outStr.find("9", pos4);
    size_t pos6 = outStr.find("6", pos9);
    EXPECT_NE(pos4, std::string::npos);
    EXPECT_NE(pos9, std::string::npos);
    EXPECT_NE(pos6, std::string::npos);
    EXPECT_LT(pos4, pos9);
    EXPECT_LT(pos9, pos6);
}
