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

    private:
        json serialize_json() const;
        void apply(Server& server);

        unsigned int target_process;
    };
} // namespace repl
