#include "REPL.h"
#include <exception>
#include <any>

// This function uses the grammar to decide whether to wrap the input.
static std::string wrapInputUsingGrammar(const std::string &code) {
    // Create an ANTLR input stream from the code.
    antlr4::ANTLRInputStream inputStream(code);

    // Run the lexer on the input.
    CLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);

    // Create the parser.
    CParser parser(&tokens);
    parser.removeErrorListeners();  // Optionally remove default error output.

    // Parse the input using your top-level rule.
    CParser::ReplInputContext* ctx = parser.replInput();

    // If the parse tree is an instance of EvaluateExpressionContext, it's a bare expression.
    if (dynamic_cast<CParser::EvaluateExpressionContext*>(ctx) != nullptr) {
        // Wrap the bare expression into a function.
        std::ostringstream wrapped;
        wrapped << "int __repl_wrapper() { return (" << code << "); }";
        return wrapped.str();
    }

    // Otherwise, assume the input is already a complete definition.
    return code;
}

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
