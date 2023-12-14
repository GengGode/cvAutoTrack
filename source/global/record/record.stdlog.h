#pragma once
#include "../global.include.h"
#include <iostream>
namespace tianli::global::record
{
    class std_logger : public logger
    {
    public:
        void log(error_type err) override
        {
            std::cout << "    |: " << err.location << " : " << err.msg << std::endl;
        }
    };
} // namespace tianli::global::log
