//
// Created by max on 08/03/25.
//

#ifndef UTILS_H
#define UTILS_H

#include "Variable.h"
#include <variant>

// A helper function to convert a VarValue to a boolean.
// You can place this in a utility header (e.g., Utils.h) or in your visitor/Interpreter source file.
bool convertToBool(const VarValue &value) {
    return std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg != 0;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg != 0.0;
        } else if constexpr (std::is_same_v<T, char>) {
            return arg != '\0';
        } else {
            // If you add more types later, handle them here.
            return false;
        }
    }, value);
}

#endif //UTILS_H
