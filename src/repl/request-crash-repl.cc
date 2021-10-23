#include "request-crash-repl.hh"

#include "raft/server.hh"

namespace repl
{
    RequestCrashREPL::RequestCrashREPL()
        : ReplMsg(REPL_MSG_TYPE::CRASH)
    {}

    json RequestCrashREPL::serialize_json() const
    {
        return ReplMsg::serialize_json();
    }

    void RequestCrashREPL::apply(Server& server)
    {
        server.set_status(ServerStatus::CRASHED);
    }
} // namespace repl
