#include "request-crash-repl.hh"

#include <iostream>

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
        try
        {
            auto& server = dynamic_cast<raft::Server&>(process);
            server.on_repl_crash();
        }
        catch (const std::bad_cast&)
        {
            std::cerr << "Crash message could not be applied\n";
        }
    }
} // namespace repl
