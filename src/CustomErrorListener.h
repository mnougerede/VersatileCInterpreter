#ifndef CUSTOM_ERROR_LISTENER_H
#define CUSTOM_ERROR_LISTENER_H

#include "antlr4-runtime.h"
#include <string>
#include <stdexcept>

class CustomErrorListener : public antlr4::BaseErrorListener {
public:
    CustomErrorListener();
    virtual ~CustomErrorListener();

    // Override the syntaxError method to handle errors.
    virtual void syntaxError(antlr4::Recognizer *recognizer,
                             antlr4::Token *offendingSymbol,
                             size_t line,
                             size_t charPositionInLine,
                             const std::string &msg,
                             std::exception_ptr e) override;
};

#endif // CUSTOM_ERROR_LISTENER_H
