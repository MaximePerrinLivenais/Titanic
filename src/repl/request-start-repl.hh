#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestStartREPL : public ReplMsg
    {
    public:
        explicit RequestStartREPL(unsigned int target_process);

        explicit RequestStartREPL(const json& json_obj);

        void apply_message(Process& process) final;

        void send();

    private:
        json serialize_json() const;

        unsigned int target_process;
    };
} // namespace repl
