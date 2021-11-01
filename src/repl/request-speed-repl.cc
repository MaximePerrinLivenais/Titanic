#include "request-speed-repl.hh"

#include <iostream>

#include "raft/server.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestSpeedREPL::RequestSpeedREPL(ServerSpeed speed)
        : ReplMsg(REPL_MSG_TYPE::SPEED)
        , speed(speed)
    {}

    RequestSpeedREPL::RequestSpeedREPL(const json& json_obj)
        : ReplMsg(json_obj["repl_msg_type"])
        , speed(json_obj["speed"])
    {}

    void RequestSpeedREPL::apply(process::Process& process)
    {
        try
        {
            auto& server = dynamic_cast<raft::Server&>(process);
            server.on_repl_speed(*this);
        }
        catch (const std::bad_cast&)
        {
            std::cerr << "Speed message could not be applied\n";
        }
    }

    json RequestSpeedREPL::serialize_json() const
    {
        json serialization = ReplMsg::serialize_json();
        serialization["speed"] = speed;

        return serialization;
    }

    ServerSpeed RequestSpeedREPL::get_speed() const
    {
        return speed;
    }

} // namespace repl
