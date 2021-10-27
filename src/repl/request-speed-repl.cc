#include "request-speed-repl.hh"

#include "raft/server.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestSpeedREPL::RequestSpeedREPL(unsigned int target_process,
            ServerSpeed speed)
        : ReplMsg(REPL_MSG_TYPE::SPEED)
        , target_process(target_process), speed(speed)
    {}

    RequestSpeedREPL::RequestSpeedREPL(const json& json_obj)
        : RequestSpeedREPL(json_obj["target_process"], json_obj["speed"])
    {}

    void RequestSpeedREPL::send()
    {
        // TODO: move this function to ReplMsg as it is the same for the 3 children class
        const std::string msg_serialized = serialize();

        MPI_Send(msg_serialized.c_str(), msg_serialized.length(), MPI_CHAR,
                 target_process, 0, MPI_COMM_WORLD);
    }

    json RequestSpeedREPL::serialize_json() const
    {
        json serialization = ReplMsg::serialize_json();
        serialization["target_process"] = target_process;
        serialization["speed"] = speed;

        return serialization;
    }

    void RequestSpeedREPL::apply(Server& server)
    {
        server.change_speed(speed);
    }
} // namespace repl
