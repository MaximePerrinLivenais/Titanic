#pragma once

#include "repl-message.hh"
#include "server-speed.hh"

namespace repl
{
    class RequestSpeedREPL : public ReplMsg
    {
    public:
        explicit RequestSpeedREPL(ServerSpeed speed);
        explicit RequestSpeedREPL(const json& json_obj);

        void apply(process::Process& process) final;

        ServerSpeed get_speed() const;

    private:
        json serialize_json() const;

        const ServerSpeed speed;
    };
} // namespace repl
