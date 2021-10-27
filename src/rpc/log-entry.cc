#include "log-entry.hh"

namespace rpc
{
    LogEntry::LogEntry(const int term, const std::string command,
            const unsigned int client_index, const unsigned int serial_number)
        : term(term)
        , command(command)
        , client_index(client_index)
        , serial_number(serial_number)
    {}

    LogEntry::LogEntry(const json& json_obj)
        : LogEntry(json_obj["term"], json_obj["command"], json_obj["client_index"],
                json_obj["serial_number"])
    {}

    const std::string LogEntry::serialize() const
    {
        json serialization = json();

        serialization["term"] = term;
        serialization["command"] = command;
        serialization["client_index"] = client_index;
        serialization["serial_number"] = serial_number;

        return serialization.dump();
    }

    std::vector<LogEntry> LogEntry::serialize(const json& json_obj)
    {
        auto entries = std::vector<LogEntry>();

        for (auto& json_element : json_obj)
            entries.emplace_back(json_element);

        return entries;
    }

    int LogEntry::get_term() const
    {
        return term;
    }

    std::string LogEntry::get_command() const
    {
        return command;
    }

    unsigned int LogEntry::get_client_index() const
    {
        return client_index;
    }

    unsigned int LogEntry::get_serial_number() const
    {
        return serial_number;
    }

    void to_json(json& json_obj, const LogEntry& log_entry)
    {
        json_obj = json{ { "term", log_entry.get_term() },
                         { "command", log_entry.get_command() },
                         { "client_index", log_entry.get_client_index() },
                         { "serial_number", log_entry.get_serial_number()}};
    }

    bool LogEntry::operator==(const LogEntry& other) const
    {
        return other.client_index == client_index &&
            other.serial_number == serial_number;
    }
} // namespace rpc
