// Function.h
#ifndef FUNCTION_H
#define FUNCTION_H

#include <string>
#include <vector>
#include "antlr4-runtime.h"

// A simple structure to represent a function.
struct Function {
    std::string returnType;                  // e.g., "int", "void", etc.
    std::vector<std::string> parameterNames; // Parameter names.
    // Optionally, you might store parameter types as well.
    // You can store the function body as an ANTLR parse tree node.
    std::string bodyText;
};

#endif // FUNCTION_H
