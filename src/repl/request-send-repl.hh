#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestSendREPL : public ReplMsg
    {
    public:
        explicit RequestSendREPL(unsigned int target_process);

        explicit RequestSendREPL(const json& json_obj);

        void send();

    private:
        json serialize_json() const;
        void apply(Server& server);

        unsigned int target_process;
    };
} // namespace repl
