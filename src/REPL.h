#ifndef REPL_H
#define REPL_H

#include <string>
#include <iostream>
#include "Interpreter.h"

class REPL {
public:
    // Original run() method uses std::cin/std::cout.
    void run();

    // Overloaded run() method for testing (or use in the IDE) that accepts streams.
    void run(std::istream &in, std::ostream &out, bool testMode);

private:
    Interpreter interpreter; // Instance of Interpreter to evaluate input.
};

#endif // REPL_H
