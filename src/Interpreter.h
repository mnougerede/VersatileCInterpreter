#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <any>
#include "antlr4-runtime.h"
#include "CLexer.h"
#include "CParser.h"
#include "CInterpreterVisitor.h"

class Interpreter {
public:
    // Evaluates a string of C code and returns the result as std::any.
    std::any evaluate(const std::string &code);
};

#endif // INTERPRETER_H
