//
// Created by max on 08/03/25.
//

#ifndef UTILS_H
#define UTILS_H

#include "Variable.h"
#include <variant>

// A helper function to convert a VarValue to a boolean.
bool convertToBool(const VarValue &value);

// Helper function to convert std::any to a string
std::string anyToString(const std::any &value);

#endif //UTILS_H
