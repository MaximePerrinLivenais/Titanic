#pragma once

#include "repl-message.hh"

namespace repl
{
    class RequestCrashREPL : public ReplMsg
    {
    public:
        RequestCrashREPL();

    private:
        json serialize_json() const;
        void apply(Server& server);
    };
} // namespace repl
