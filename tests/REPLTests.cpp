#include "gtest/gtest.h"
#include "REPL.h"
#include <sstream>
#include <string>

TEST(REPLTest, RunValidInput) {
    // Simulate input: a valid expression followed by "exit".
    std::istringstream input("3 + 4;\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output);

    std::string outStr = output.str();
    // Verify that the output includes "Result: 7"
    EXPECT_NE(outStr.find("Result: 7"), std::string::npos);
}

TEST(REPLTest, RunInvalidInput) {
    // Simulate input with an invalid expression.
    std::istringstream input("3 + ;\nexit\n");
    std::ostringstream output;

    REPL repl;
    repl.run(input, output);

    std::string outStr = output.str();
    // Verify that an error message is printed.
    EXPECT_NE(outStr.find("Error:"), std::string::npos);
}
