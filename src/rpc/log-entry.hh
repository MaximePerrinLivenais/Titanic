#pragma once

#include <string>

namespace rpc
{
    struct LogEntry
    {
        explicit LogEntry(const std::string command, const unsigned int term);

        std::string command;
        unsigned int term;
    };
} // namespace rpc
