#ifndef UTILS_H
#define UTILS_H

#include <any>

#include "Variable.h"
#include <variant>
#include <string>

// A helper function to convert a VarValue to a boolean.
bool convertToBool(const VarValue &value);

// Convert any std::any to a string (for REPL output), including std::string
std::string anyToString(const std::any &value);

// Trim whitespace from both ends
std::string trim(const std::string &s);


#endif // UTILS_H
