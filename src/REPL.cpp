#include "REPL.h"
#include <exception>
#include <any>
#include <typeinfo>

#include "Utils.h"
#include "Variable.h"


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
            std::any result = interpreter.evaluate(input, false);

            // Convert result to a string using the helper function.
            std::string output = anyToString(result);

            if (testMode) {
                out << output;
            } else {
                out << output << "\n\n> ";
            }
        } catch (const std::exception &e) {
            if (testMode) {
                out << "Error: " << e.what();
            } else {
                out << "Error: " << e.what() << "\n\n> ";
            }
        }
    }
}
// Evaluate a single command
std::string REPL::evaluateCommand(const std::string &input) {
    try {
        std::any result = interpreter.evaluate(input, false);
        return anyToString(result);
    } catch (const std::exception &e) {
        return std::string("Error: ") + e.what();
    }
}