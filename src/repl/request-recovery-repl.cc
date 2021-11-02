#include "request-recovery-repl.hh"

#include "raft/server.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestRecoveryREPL::RequestRecoveryREPL()
        : ReplMsg(REPL_MSG_TYPE::RECOVERY)
    {}

    RequestRecoveryREPL::RequestRecoveryREPL(const json& json_obj)
        : ReplMsg(json_obj["repl_msg_type"])
    {}

    void RequestRecoveryREPL::apply(process::Process& process)
    {
        try
        {
            auto& server = dynamic_cast<raft::Server&>(process);
            server.on_repl_recovery();
        }
        catch (const std::bad_cast&)
        {
            std::cerr << "Recovery message could not be applied\n";
        }
    }
} // namespace repl
