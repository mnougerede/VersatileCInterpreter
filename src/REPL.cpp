#include "REPL.h"
#include <exception>
#include <any>
#include <typeinfo>

#include "Utils.h"
#include "Variable.h"

#define LOG(x) std::clog << x << std::endl;

// Original run() using std::cin and std::cout.
void REPL::run() {
    run(std::cin, std::cout, false);
}

// Overloaded run() that accepts input and output streams.
void REPL::run(std::istream &in, std::ostream &out, bool testMode) {
    std::string line;
    if (!testMode) {
        out << "Enter an expression (or 'exit' to quit):\n> ";
    }

    while (std::getline(in, line)) {
        LOG("[REPL] Read line: '" << line << "'");

        // Trim whitespace on both ends so that "exit " or " exit" will match.
        auto trimmed = trim(line);
        LOG("[REPL] Trimmed line: '" << trimmed << "'");

        if (trimmed == "exit" || trimmed == "quit") {
            LOG("[REPL] Exit command detected, breaking loop.");
            break;
        }
        if (trimmed.empty()) {
            LOG("[REPL] Empty input, skipping.");
            if (!testMode) out << "Empty input, please try again.\n\n> ";
            continue;
        }

        try {
            LOG("[REPL] Evaluating: " << trimmed);
            std::any result = interpreter.evaluate(trimmed, false);
            std::string outStr = anyToString(result);
            LOG("[REPL] Result: " << outStr);

            if (testMode) {
                out << outStr;
            } else {
                out << outStr << "\n\n> ";
            }
        } catch (const std::exception &e) {
            LOG("[REPL] Caught exception: " << e.what());
            if (testMode) {
                out << "Error: " << e.what();
            } else {
                out << "Error: " << e.what() << "\n\n> ";
            }
        }
    }
    LOG("[REPL] Exiting run()");
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