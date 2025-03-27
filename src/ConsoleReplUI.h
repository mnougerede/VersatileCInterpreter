// ConsoleReplUI.h
#pragma once
#include "IReplUI.h"
#include "REPL.h"  // your existing REPL class

class ConsoleReplUI : public IReplUI {
public:
    ConsoleReplUI() = default;
    void run() override {
        REPL repl;
        repl.run();  // uses std::cin/std::cout
    }
};
