#include "request-start-repl.hh"

#include "client/client.hh"
#include "raft/server.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestStartREPL::RequestStartREPL(unsigned int target_process)
        : ReplMsg(REPL_MSG_TYPE::START)
        , target_process(target_process)
    {}

    RequestStartREPL::RequestStartREPL(const json& json_obj)
        : ReplMsg(json_obj["repl_msg_type"])
        , target_process(json_obj["target_process"])
    {}

    void RequestStartREPL::send()
    {
        // TODO: move to ReplMsg
        const std::string msg_serialized = serialize();

        MPI_Send(msg_serialized.c_str(), msg_serialized.length(), MPI_CHAR,
                 target_process, 0, MPI_COMM_WORLD);
    }

    json RequestStartREPL::serialize_json() const
    {
        json serialization = ReplMsg::serialize_json();
        serialization["target_process"] = target_process;

        return serialization;
    }

    void RequestStartREPL::apply(Process& process)
    {
        auto& client = dynamic_cast<Client&>(process);
        client.on_repl_start();
    }
} // namespace repl
