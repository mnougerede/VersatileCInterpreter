#ifndef VARIABLE_H
#define VARIABLE_H
#include <variant>

// Enum for variable types.
enum class VarType { INT, FLOAT, DOUBLE, VOID, CHAR };

// Variant type to store variable values. For now, we use int, double, char, and bool.
using VarValue = std::variant<int, double, char>;

struct Variable {
    VarType type;
    VarValue value;
};

#endif // VARIABLE_H
