// ReturnException.h
#ifndef RETURN_EXCEPTION_H
#define RETURN_EXCEPTION_H

#include <exception>
#include "Variable.h"    // for VarValue

class ReturnException : public std::exception {
public:
    // take a VarValue directly
    ReturnException(VarValue v)
      : value(v)
    {}

    // return the VarValue
    VarValue getValue() const {
        return value;
    }

private:
    VarValue value;
};

#endif
