#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestRecoveryREPL : public ReplMsg
    {
    public:
        explicit RequestRecoveryREPL();
        explicit RequestRecoveryREPL(const json& json_obj);

        void apply(process::Process& process) final;
    };
} // namespace repl
