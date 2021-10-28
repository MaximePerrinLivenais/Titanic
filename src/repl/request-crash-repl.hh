#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestCrashREPL : public ReplMsg
    {
    public:
        explicit RequestCrashREPL(unsigned int target_process);

        explicit RequestCrashREPL(const json& json_obj);

        void send();

        void apply_message(Process& process) final;

    private:
        json serialize_json() const;

        unsigned int target_process;
    };
} // namespace repl
