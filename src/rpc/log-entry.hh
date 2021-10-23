#pragma once

#include <string>
#include <vector>

#include "misc/json.hh"

namespace rpc
{
    class LogEntry
    {
    public:
        LogEntry() = default;
        explicit LogEntry(const std::string command, const int term);
        explicit LogEntry(const json& json_obj);

        const std::string serialize() const;
        static std::vector<LogEntry> serialize(const json& json_obj);

        int get_term() const;
        std::string get_command() const;

    private:
        unsigned int term;
        std::string command;
    };
} // namespace rpc
