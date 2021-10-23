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
        explicit LogEntry(const int term, const std::string command);
        explicit LogEntry(const json& json_obj);

        const std::string serialize() const;
        static std::vector<LogEntry> serialize(const json& json_obj);

        int get_term() const;
        std::string get_command() const;

    private:
        unsigned int term;
        std::string command;
    };

    void to_json(json& json_obj, const LogEntry& log_entry);
} // namespace rpc
