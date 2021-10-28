#pragma once

#include "repl-message.hh"
#include "repl.hh"

namespace repl
{
    class RequestSpeedREPL : public ReplMsg
    {
    public:
        explicit RequestSpeedREPL(unsigned int target_process,
                                  ServerSpeed speed);

        explicit RequestSpeedREPL(const json& json_obj);

        void send();

        void apply_message(Process& process) final;

    private:
        json serialize_json() const;

        unsigned int target_process;
        ServerSpeed speed;
    };
} // namespace repl
