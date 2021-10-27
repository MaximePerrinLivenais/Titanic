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
        explicit LogEntry(const int term, const std::string command,
            const unsigned int client_index, const unsigned int serial_number);
        explicit LogEntry(const json& json_obj);

        const std::string serialize() const;
        static std::vector<LogEntry> serialize(const json& json_obj);

        int get_term() const;
        std::string get_command() const;
        unsigned int get_client_index() const;
        unsigned int get_serial_number() const;


        bool operator==(const LogEntry& other) const;

    private:
        unsigned int term;
        std::string command;

        unsigned int client_index;
        unsigned int serial_number;
    };

    void to_json(json& json_obj, const LogEntry& log_entry);
} // namespace rpc
