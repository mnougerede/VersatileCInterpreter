#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <unordered_map>
#include <string>
#include <stdexcept>

#include "Variable.h"
#include "Function.h"

class Environment {
public:
    // Constructor. Optionally provide a parent for nested scopes.
    Environment(Environment* parentEnv = nullptr);

    // Set a variable in the current scope.
    void define(const std::string &name, VarType type, const std::variant<int, double, char> value);

    void assign(const std::string &name, VarType type, const std::variant<int, double, char>& value);

    // Get a variable's value, checking outer scopes if needed.
    Variable get(const std::string &name) const;

    // Check if a variable exists in this scope or outer scopes.
    bool exists(const std::string &name) const;

    // pushScope returns a pointer to the new environment (child scope).
    Environment* pushScope();
    // popScope returns the parent environment (exiting the current scope).
    Environment* popScope();

    void defineFunction(const std::string &name, const Function &func);
    Function* getFunction(const std::string &name);
    bool functionExists(const std::string &name) const;

private:
    std::unordered_map<std::string, Variable> variables;
    std::unordered_map<std::string, Function> functions;
    //TODO considering upgrading to a smart pointer
    Environment* parent;  // Parent scope (nullptr for global scope).
};

#endif // ENVIRONMENT_H
