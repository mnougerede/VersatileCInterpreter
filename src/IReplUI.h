// IReplUI.h
#pragma once
#include <string>

class IReplUI {
public:
    virtual ~IReplUI() = default;
    virtual void run() = 0;
};
