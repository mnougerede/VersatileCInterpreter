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
