#include "Interpreter.h"
#include "CustomErrorListener.h"
#include "EnvScopeGuard.h"
#include "ReturnException.h"

Interpreter::Interpreter() {
    globalEnv = new Environment(nullptr); // Global environment; no parent.
}

std::any Interpreter::evaluate(const std::string &code, bool isFileMode) {
    // Create the input stream from the code.
    antlr4::ANTLRInputStream inputStream(code);
    CLexer lexer(&inputStream);
    antlr4::CommonTokenStream tokens(&lexer);
    CParser parser(&tokens);

    parser.removeErrorListeners();
    CustomErrorListener errorListener;
    parser.addErrorListener(&errorListener);

    std::any rawResult;
    if (isFileMode) {
        // For file mode, require a complete translation unit.
        auto* tree = parser.translationUnit();
        CInterpreterVisitor visitor(globalEnv, &tokens);
        visitor.visit(tree); // register functions etc

        // Now lookup and call main.
        Function* mainFunc = globalEnv->getFunction("main");
        if (!mainFunc) {
            throw std::runtime_error("No main function defined.");
        }

        antlr4::ANTLRInputStream  bodyIn(mainFunc->bodyText);
        CLexer                   bodyLex(&bodyIn);
        antlr4::CommonTokenStream bodyTokens(&bodyLex);
        CParser                  bodyParser(&bodyTokens);

        auto* bodyCtx = bodyParser.compoundStatement();

        // Create a new scope to execute main.
        {
            EnvScopeGuard guard(globalEnv);
            try {
                rawResult = visitor.visit(bodyCtx);
            } catch (const ReturnException &retEx) {
                rawResult = retEx.getValue();
            }
        }



    } else {
        // For REPL mode, be more flexible.
        CParser::ReplInputContext *tree = parser.replInput();
        CInterpreterVisitor visitor(globalEnv, &tokens);
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

Interpreter::~Interpreter() {
    delete globalEnv; // Clean up the dynamically allocated global environment.
}
