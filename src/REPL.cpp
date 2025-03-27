#include "REPL.h"
#include <exception>
#include <any>
#include <typeinfo>
#include "Variable.h"
// Helper function to convert std::any to a string
std::string anyToString(const std::any &value) {
    try {
        if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(char)) {
            return std::string(1, std::any_cast<char>(value));
        } else if (value.has_value()) {
            return "[Unknown type]";
        } else {
            return "[No value]";
        }
    } catch (const std::bad_any_cast &) {
        return "[Type conversion error]";
    }
}

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
        std::any result = interpreter.evaluate(input);
        return anyToString(result);
    } catch (const std::exception &e) {
        return std::string("Error: ") + e.what();
    }
}