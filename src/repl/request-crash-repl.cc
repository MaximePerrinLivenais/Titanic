#include "request-crash-repl.hh"

#include "raft/server.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestCrashREPL::RequestCrashREPL(unsigned int target_process)
        : ReplMsg(REPL_MSG_TYPE::CRASH)
        , target_process(target_process)
    {}

    RequestCrashREPL::RequestCrashREPL(const json& json_obj)
        : ReplMsg(json_obj["repl_msg_type"])
        , target_process(json_obj["target_process"])
    {}

    void RequestCrashREPL::send()
    {
        const std::string msg_serialized = serialize();

        MPI_Send(msg_serialized.c_str(), msg_serialized.length(), MPI_CHAR,
                 target_process, 0, MPI_COMM_WORLD);
    }

    json RequestCrashREPL::serialize_json() const
    {
        json serialization = ReplMsg::serialize_json();
        serialization["target_process"] = target_process;

        return serialization;
    }

    void RequestCrashREPL::apply(Process& process)
    {
        Server& server = dynamic_cast<Server&>(process);
        server.crash();
    }

} // namespace repl
