#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <stdexcept>

#include "Variable.h"

class Environment {
public:
    // Constructor. Optionally provide a parent for nested scopes.
    Environment(Environment* parentEnv = nullptr);

    // Set a variable in the current scope.
    void set(const std::string &name, VarType type, const std::variant<std::monostate, int, double, char> value);

    // Get a variable's value, checking outer scopes if needed.
    Variable get(const std::string &name) const;

    // Check if a variable exists in this scope or outer scopes.
    bool exists(const std::string &name) const;

private:
    std::unordered_map<std::string, Variable> variables;
    //TODO considering upgrading to a smart pointer
    Environment* parent;  // Parent scope (nullptr for global scope).
};

#endif // ENVIRONMENT_H
