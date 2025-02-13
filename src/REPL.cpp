#include "REPL.h"
#include <iostream>
#include <exception>
#include <any>

void REPL::run() {
    std::string input;
    std::cout << "Enter an expression (or 'exit' to quit):\n> ";

    while (std::getline(std::cin, input)) {
        if (input == "exit" || input == "quit") {
            break;
        }

        try {
            // Evaluate the input using the interpreter.
            std::any result = interpreter.evaluate(input);

            // Attempt to cast the result to a double (if applicable).
            double val = std::any_cast<double>(result);
            std::cout << "Result: " << val << "\n\n> ";
        }
        catch (const std::bad_any_cast &e) {
            std::cerr << "Result has an unexpected type.\n\n> ";
        }
        catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << "\n\n> ";
        }
    }
}
