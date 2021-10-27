#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestRecoveryREPL : public ReplMsg
    {
    public:
        explicit RequestRecoveryREPL(unsigned int target_process);

        explicit RequestRecoveryREPL(const json& json_obj);

        void send();

    private:
        json serialize_json() const;
        void apply(Server& server);

        unsigned int target_process;
    };
} // namespace repl
