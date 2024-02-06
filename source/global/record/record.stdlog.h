#pragma once
#include "../global.include.h"
#include <iostream>
#include <map>
#include <chrono>

namespace tianli::global::record
{
    class std_logger : public logger
    {
    public:
        void log(error_type err) override { std::cout << "    |: " << err.location << " : " << err.msg << std::endl; }

        void perl(std::string perl_label = "default") override
        {
            auto now = std::chrono::system_clock::now();
            perl_map[perl_label] = now;
            current_perl_it = perl_map.find(perl_label);
        }
        void perl_end(std::string perl_label = "default") override
        {
            auto now = std::chrono::system_clock::now();
            if (perl_label.empty())
            {
                if (current_perl_it == perl_map.end())
                    return;
                std::cout << "    |: " << current_perl_it->first << " : " << std::chrono::duration_cast<std::chrono::milliseconds>(now - current_perl_it->second).count() << "ms" << std::endl;
                perl_map.erase(current_perl_it);
                current_perl_it = perl_map.end();
            }
            else
            {
                auto it = perl_map.find(perl_label);
                if (it == perl_map.end())
                    return;
                std::cout << "    |: " << it->first << " : " << std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count() << "ms" << std::endl;
                perl_map.erase(it);
            }
        }

    private:
        std::map<std::string, std::chrono::system_clock::time_point> perl_map;
        std::map<std::string, std::chrono::system_clock::time_point>::iterator current_perl_it = perl_map.end();
    };
} // namespace tianli::global::record
