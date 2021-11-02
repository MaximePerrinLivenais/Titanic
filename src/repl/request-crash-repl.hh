#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestCrashREPL : public ReplMsg
    {
    public:
        explicit RequestCrashREPL();
        explicit RequestCrashREPL(const json& json_obj);

        void apply(process::Process& process) final;
    };
} // namespace repl
