#pragma once

#include "repl-message.hh"
#include "server-speed.hh"

namespace repl
{
    class RequestSpeedREPL : public ReplMsg
    {
    public:
        explicit RequestSpeedREPL(unsigned int target_process, ServerSpeed speed);
        explicit RequestSpeedREPL(const json& json_obj);

        void apply(process::Process& process) final;

        void send();

        ServerSpeed get_speed() const;

    private:
        json serialize_json() const;

        const unsigned int target_process;
        const ServerSpeed speed;
    };
} // namespace repl
