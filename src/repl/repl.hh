#pragma once

#include <optional>
#include <string>
#include "server-speed.hh"
#include "process/process.hh"

namespace repl
{
    class REPL : public process::Process
    {
    public:
        REPL();

        void run();
    };

    std::optional<ServerSpeed> str_to_server_speed(const std::string& speed_str);
} // namespace repl
