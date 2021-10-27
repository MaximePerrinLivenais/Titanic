#pragma once

#include "repl.hh"
#include "repl-message.hh"

namespace repl
{
    class RequestSpeedREPL : public ReplMsg
    {
    public:
        explicit RequestSpeedREPL(unsigned int target_process, ServerSpeed speed);

        explicit RequestSpeedREPL(const json& json_obj);

        void send();

    private:
        json serialize_json() const;
        void apply(Server& server);

        unsigned int target_process;
        ServerSpeed speed;
    };
} // namespace repl
