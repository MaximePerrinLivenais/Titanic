#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestSendREPL : public ReplMsg
    {
    public:
        explicit RequestSendREPL();
        explicit RequestSendREPL(const json& json_obj);

        void apply(process::Process& process);
    };
} // namespace repl
