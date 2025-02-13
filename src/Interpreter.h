//
// Created by max on 05/02/25.
//

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include "antlr4-runtime.h"
#include "CInterpreterVisitor.h"
#include "CParser.h"
#include "CLexer.h"

class Interpreter {
public:
    Interpreter();  // Constructor for setup if needed
    int evaluate(const std::string& code);  // Evaluates an expression and returns an int

private:
    antlr4::ANTLRInputStream inputStream;
    CInterpreterVisitor visitor;
};


#endif //INTERPRETER_H
