#include "Interpreter.h"
#include "CustomErrorListener.h"

std::any Interpreter::evaluate(const std::string &code) {
    // Create the input stream from the code.
    antlr4::ANTLRInputStream inputStream(code);

    // Lexical analysis.
    CLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);

    // Parse the tokens.
    CParser parser(&tokens);

    // Remove default error listeners.
    parser.removeErrorListeners();
    // Attach our custom error listener.
    CustomErrorListener errorListener;
    parser.addErrorListener(&errorListener);

    // Use the top-level rule (assuming 'replInput' is defined in your grammar).
    CParser::ReplInputContext *tree = parser.replInput();

    // Create our visitor.
    CInterpreterVisitor visitor;

    // Visit the parse tree and return the result.
    return visitor.visit(tree);
}
