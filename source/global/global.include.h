#pragma once
#include <fmt/format.h>

namespace tianli::global
{
    class error_logger
    {
        error_logger() = default;
        virtual ~error_logger() = default;

        template <typename... Args>
        constexpr auto format(std::string_view fmt, Args &&...args)
        {
            return fmt::format(fmt, fmt::make_format_args(std::forward<Args>(args)...));
        }

        virtual void error(std::string_view log) = 0;
    };
} // namespace tianli::global
