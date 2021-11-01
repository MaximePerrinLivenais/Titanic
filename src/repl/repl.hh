#pragma once

#include <optional>
#include <string>

namespace repl
{
    enum ServerSpeed
    {
        LOW,
        MEDIUM,
        HIGH
    };

    class REPL
    {
    public:
        REPL() = default;

        void run();

        // void send_crash_request();

    private:
        // XXX: List of Process (Server + client)
    };

    std::optional<ServerSpeed> str_to_server_speed(const std::string& speed_str);
    bool is_valid_command(const std::string& command);
} // namespace repl
