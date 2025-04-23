// Function.h
#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include "antlr4-runtime.h"

// A simple structure to represent a function.
struct Function {
    VarType returnType;                           // e.g. VarType::INT, .DOUBLE, .CHAR
    std::vector<VarType> parameterTypes;          // parallel to
    std::vector<std::string> parameterNames;      // parameterNames
    std::string bodyText;
};


#endif // FUNCTION_H
