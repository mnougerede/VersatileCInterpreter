// main.cpp
#include "IReplUI.h"
#include "ConsoleReplUI.h"
#include "ImGuiReplUI.h"
#include <iostream>
#include <memory>

int main(int argc, char** argv) {
    std::unique_ptr<IReplUI> ui;
    // if (argc > 1 && std::string(argv[1]) == "--gui") {
    //     ui = std::make_unique<ImGuiReplUI>();
    // } else {
    //     ui = std::make_unique<ConsoleReplUI>();
    // }

    // ui = std::make_unique<ConsoleReplUI>();
    ui = std::make_unique<ImGuiReplUI>();
    ui->run();
    return 0;
}
