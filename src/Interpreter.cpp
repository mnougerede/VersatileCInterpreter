#include "Interpreter.h"
#include "CustomErrorListener.h"
Interpreter::Interpreter()
{}

std::any Interpreter::evaluate(const std::string &code, bool isFileMode) {
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

    std::any rawResult;
    if (isFileMode) {
        // For file mode, require a complete translation unit.
        CParser::TranslationUnitContext *tree = parser.translationUnit();
        CInterpreterVisitor visitor(&globalEnv);
        rawResult = visitor.visit(tree);
    } else {
        // For REPL mode, be more flexible.
        CParser::ReplInputContext *tree = parser.replInput();
        CInterpreterVisitor visitor(&globalEnv);
        rawResult = visitor.visit(tree);
    }

    // if the rawResult holds a VarValue, unwrap it.
    if (rawResult.type() == typeid(VarValue)) {
        VarValue value = std::any_cast<VarValue>(rawResult);
        // Unwrap the variant and return the plain type.
        if (std::holds_alternative<int>(value)) {
            return std::any(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return std::any(std::get<double>(value));
        } else if (std::holds_alternative<char>(value)) {
            return std::any(std::get<char>(value));
        } else {
            // Shouldn't happen, unless you add new alternatives.
            return {};
        }
    }
    // If rawResult isn't a VarValue, assume it's already a plain type.
    return rawResult;
}
