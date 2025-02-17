#include "REPL.h"
#include <exception>
#include <any>

// Original run() using std::cin and std::cout.
void REPL::run() {
    run(std::cin, std::cout, false);
}

// Overloaded run() that accepts input and output streams.
void REPL::run(std::istream &in, std::ostream &out, bool testMode) {
    std::string input;
    if (!testMode) {
        out << "Enter an expression (or 'exit' to quit):\n> ";
    }

    while (std::getline(in, input)) {
        if (input == "exit" || input == "quit") {
            break;
        }
        if (input.empty()) {
            out << "Empty input, please try again.\n\n>";
            continue;
        }

        try {
            // Evaluate the input using the interpreter.
            std::any result = interpreter.evaluate(input);

            // For our tests, we assume the result is a double.
            auto val = std::any_cast<double>(result);
            if (testMode) {
                out << val;
            } else {
                out << val << "\n\n> ";
            }
        }
        catch (const std::bad_any_cast &) {
            if (testMode) {
                out << "Result has an unexpected type.";
            } else {
                out << "Result has an unexpected type.\n\n> ";
            }
        }
        catch (const std::exception &e) {
            if (testMode) {
                out << "Error: " << e.what();
            } else {
                out << "Error: " << e.what() << "\n\n> ";
            }
        }
    }
}