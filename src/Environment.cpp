//
// Created by max on 17/02/25.
//

#include "Environment.h"

Environment::Environment(Environment *parentEnv)
    : parent(parentEnv) {}

void Environment::define(const std::string &name, VarType type, const std::variant<int, double, char> value) {
    variables[name] = {type, value};
}

void Environment::assign(const std::string &name, VarType type, const std::variant<int, double, char>& value) {
    // Check if the variable exists in this scope.
    if (variables.find(name) != variables.end()) {
        variables[name] = {type, value};
    } else if (parent != nullptr) {
        // Recurse into the parent environment.
        parent->assign(name, type, value);
    } else {
        throw std::runtime_error("Undefined variable: " + name);
    }
}

Variable Environment::get(const std::string &name) const {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    } else if (parent != nullptr) {
        return parent->get(name);
    }
    throw std::runtime_error("Undefined variable: " + name);
}

bool Environment::exists(const std::string &name) const {
    if (variables.find(name) != variables.end())
        return true;
    if (parent != nullptr)
        return parent->exists(name);
    return false;
}
Environment* Environment::pushScope() {
    // Create a new Environment with this as the parent.
    return new Environment(this);
}

Environment* Environment::popScope() {
    // When popping, simply return the parent pointer.
    // Memory management is something you'll need to handle carefully
    // (consider using smart pointers).
    return parent;
}

// Function-related methods
void Environment::defineFunction(const std::string &name, const Function &func) {
    functions[name] = func;
}
Function* Environment::getFunction(const std::string &name) {
    auto it = functions.find(name);
    if (it != functions.end()) {
        return &it->second;
    } else if (parent != nullptr) {
        return parent->getFunction(name);
    }
    return nullptr;
}
bool Environment::functionExists(const std::string &name) const {
    if (functions.find(name) != functions.end()) {
        return true;
    }
    if (parent != nullptr) {
        return parent->functionExists(name);
    }
    return false;
}