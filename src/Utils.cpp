#include "Utils.h"
#include "Variable.h"

#include <algorithm>
#include <any>
#include <string>

// A helper function to convert a VarValue to a boolean.
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
// Trim whitespace from both ends
std::string trim(const std::string &s) {
    auto ws_front = std::find_if_not(s.begin(), s.end(), ::isspace);
    auto ws_back  = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
    return (ws_front < ws_back
            ? std::string(ws_front, ws_back)
            : std::string());
}
// Helper function to convert std::any to a string
std::string anyToString(const std::any &value) {
    try {
        if (value.type() == typeid(int)) {
            return std::to_string(std::any_cast<int>(value));
        } else if (value.type() == typeid(double)) {
            return std::to_string(std::any_cast<double>(value));
        } else if (value.type() == typeid(char)) {
            return std::string(1, std::any_cast<char>(value));
        } else if (value.has_value()) {
            return "[Unknown type]";
        } else {
            return "[No value]";
        }
    } catch (const std::bad_any_cast &) {
        return "[Type conversion error]";
    }
}
