#pragma once
#include <memory>
#include <string>
struct debugger
{
    struct impl_t;
    std::unique_ptr<impl_t> impl;
    debugger();
    ~debugger();

    void initlize();
    void destory();
    std::string call(std::string command, std::string args);
};