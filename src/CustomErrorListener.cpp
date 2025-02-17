#include "CustomErrorListener.h"
#include <sstream>

CustomErrorListener::CustomErrorListener() {
    // Constructor (if needed)
}

CustomErrorListener::~CustomErrorListener() {
    // Destructor (if needed)
}

void CustomErrorListener::syntaxError(antlr4::Recognizer *recognizer,
                                        antlr4::Token *offendingSymbol,
                                        size_t line,
                                        size_t charPositionInLine,
                                        const std::string &msg,
                                        std::exception_ptr e) {
    std::ostringstream errorStream;
    errorStream << "Syntax error at line " << line
                << ", column " << charPositionInLine
                << ": " << msg;
    // Throw a runtime_error with the error message.
    throw std::runtime_error(errorStream.str());
}
