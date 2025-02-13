#include <iostream>
#include "antlr4-runtime.h"
#include "CLexer.h"
#include "CParser.h"
#include "CInterpreterVisitor.h"

int main() {
    std::string input;
    std::cout << "Enter an expression (or 'exit' to quit):\n> ";

    while (std::getline(std::cin, input)) {
        if (input == "exit") {
            break;
        }

        try {
            // Create the input stream
            antlr4::ANTLRInputStream inputStream(input);

            // Lexer
            CLexer lexer(&inputStream);
            antlr4::CommonTokenStream tokens(&lexer);

            // Parser
            CParser parser(&tokens);

            // Parse using the top-level rule from your grammar, e.g. 'replInput'
            CParser::ReplInputContext *tree = parser.replInput();

            // Create our interpreter visitor
            CInterpreterVisitor interpreter;

            // Visit the tree
            std::any result = interpreter.visit(tree);

            // Try casting the result to a double
            double val = std::any_cast<double>(result);
            std::cout << "Result: " << val << "\n\n> ";
        }
        catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << "\n\n> ";
        }
    }

    return 0;
}
