#include "REPL.h"
#include <exception>
#include <any>

// Original run() using std::cin and std::cout.
void REPL::run() {
    run(std::cin, std::cout);
}

// Overloaded run() that accepts input and output streams.
void REPL::run(std::istream &in, std::ostream &out) {
    std::string input;
    out << "Enter an expression (or 'exit' to quit):\n> ";

    while (std::getline(in, input)) {
        if (input == "exit") {
            break;
        }

        try {
            // Evaluate the input using the interpreter.
            std::any result = interpreter.evaluate(input);

            // For our tests, we assume the result is an int.
            int val = std::any_cast<int>(result);
            out << "Result: " << val << "\n\n> ";
        }
        catch (const std::bad_any_cast &) {
            out << "Result has an unexpected type.\n\n> ";
        }
        catch (const std::exception &e) {
            out << "Error: " << e.what() << "\n\n> ";
        }
    }
}
