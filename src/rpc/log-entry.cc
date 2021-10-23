#include "log-entry.hh"

namespace rpc
{
    LogEntry::LogEntry(const int term, const std::string command)
        : term(term)
        , command(command)
    {}

    LogEntry::LogEntry(const json& json_obj)
        : LogEntry(json_obj["term"], json_obj["command"])
    {}

    const std::string LogEntry::serialize() const
    {
        json serialization = json();

        serialization["term"] = term;
        serialization["command"] = command;

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

    void to_json(json& json_obj, const LogEntry& log_entry)
    {
        json_obj = json{ { "term", log_entry.get_term() },
                         { "command", log_entry.get_command() } };
    }
} // namespace rpc
