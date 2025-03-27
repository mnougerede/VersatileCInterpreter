// ImGuiReplUI.h
#pragma once
#include "IReplUI.h"
#include "REPL.h"  // for evaluation logic (or you can use a dedicated method)
#include <string>

class ImGuiReplUI : public IReplUI {
public:
    ImGuiReplUI();
    ~ImGuiReplUI();
    void run() override;

private:
    // Internal state for the REPL window.
    std::string replOutput;
    char replInput[512];
    // You can also hold an instance of your REPL for evaluation
    REPL repl;

    // Initialization & cleanup helper methods.
    bool init();
    void cleanup();
};
