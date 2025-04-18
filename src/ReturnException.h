#ifndef RETURN_EXCEPTION_H
#define RETURN_EXCEPTION_H

#include <stdexcept>
#include <any>

class ReturnException : public std::runtime_error {
public:
    // Constructor accepts a return value.
    ReturnException(const std::any &value)
        : std::runtime_error("Function returned"), returnValue(value) {}

    // Retrieve the return value.
    std::any getValue() const {
        return returnValue;
    }

private:
    std::any returnValue;
};

#endif // RETURN_EXCEPTION_H
