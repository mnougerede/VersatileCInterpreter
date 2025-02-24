//
// Created by max on 17/02/25.
//

#include "Environment.h"

Environment::Environment(Environment *parentEnv)
    : parent(parentEnv) {}

void Environment::set(const std::string &name, VarType type, const std::variant<int, double, char> value) {
    variables[name] = {type, value};
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
