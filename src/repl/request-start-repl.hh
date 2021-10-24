#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestStartREPL : public ReplMsg
    {
    public:
        explicit RequestStartREPL(unsigned int target_process);

        explicit RequestStartREPL(const json& json_obj);

        void send();

    private:
        json serialize_json() const;
        void apply(Server& server);

        unsigned int target_process;
    };
} // namespace repl
