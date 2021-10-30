#include "request-crash-repl.hh"

#include "raft/server.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestCrashREPL::RequestCrashREPL()
        : ReplMsg(REPL_MSG_TYPE::CRASH)
    {}

    RequestCrashREPL::RequestCrashREPL(const json& json_obj)
        : ReplMsg(json_obj["repl_msg_type"])
    {}

    void RequestCrashREPL::apply(process::Process& process)
    {
        auto& server = dynamic_cast<raft::Server&>(process);
        server.on_repl_crash();
    }

    json RequestCrashREPL::serialize_json() const
    {
        json serialization = ReplMsg::serialize_json();

        return serialization;
    }

} // namespace repl
