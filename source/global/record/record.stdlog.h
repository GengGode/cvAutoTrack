#pragma once
#include "../global.include.h"
#include <iostream>
namespace tianli::global::record
{
    class std_logger : public logger
    {
    public:
        void log(error_info err) override
        {
            std::cout << "    |: " << err.file << " : " << err.msg << std::endl;
        }
    };
} // namespace tianli::global::log
