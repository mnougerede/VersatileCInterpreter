#include "gtest/gtest.h"
#include "REPL.h"
#include <sstream>
#include <string>

TEST(REPLTest, RunValidInput) {
    // Simulate input: a valid expression followed by "exit".
    std::istringstream input("3 + 4\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    EXPECT_EQ(outStr, "7");

}

TEST(REPLTest, RunInvalidInput) {
    // Simulate input with an invalid expression.
    std::istringstream input("3 + \nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output,true);

    std::string outStr = output.str();
    // Verify that an error message is printed.
    EXPECT_NE(outStr.find("Error:"), std::string::npos);
}
// Test that the REPL stops on "quit" as well as "exit".
TEST(REPLTest, QuitTerminatesSession) {
    std::istringstream input("5 * 2\nquit\n");
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
    std::istringstream input("3 * 3\n\n4 + 1\nexit\n");
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
    std::istringstream input("2 + 2\n7 - 3\n10 / 2\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output, true);

    std::string outStr = output.str();
    // We expect the results concatenated in order: "4", "4", "5"
    // The test mode is set up to not have new lines, so results will be concatenated.
    EXPECT_NE(outStr.find("4"), std::string::npos);
    EXPECT_NE(outStr.find("5"), std::string::npos);

}