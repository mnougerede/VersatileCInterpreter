#ifndef REPL_H
#define REPL_H

#include <string>
#include "Interpreter.h"

class REPL {
public:
    // Runs the REPL loop.
    void run();
private:
    Interpreter interpreter; // Instance of Interpreter to evaluate input.
};

#endif // REPL_H
