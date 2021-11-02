#include "request-start-repl.hh"

#include <iostream>

#include "client/client.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestStartREPL::RequestStartREPL()
        : ReplMsg(REPL_MSG_TYPE::START)
    {}

    RequestStartREPL::RequestStartREPL(const json& json_obj)
        : ReplMsg(json_obj["repl_msg_type"])
    {}

    void RequestStartREPL::apply(process::Process& process)
    {
        try
        {
            auto& client = dynamic_cast<client::Client&>(process);
            client.on_repl_start();
        }
        catch (const std::bad_cast&)
        {
            std::cerr << "Start message could not be applied\n";
        }
    }

} // namespace repl
