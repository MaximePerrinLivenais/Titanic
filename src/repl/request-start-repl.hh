#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestStartREPL : public ReplMsg
    {
    public:
        explicit RequestStartREPL();
        explicit RequestStartREPL(const json& json_obj);

        void apply(process::Process& process) final;
    };
} // namespace repl
