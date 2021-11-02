#include "request-send-repl.hh"

#include "client/client.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    RequestSendREPL::RequestSendREPL()
        : ReplMsg(REPL_MSG_TYPE::SEND)
    {}

    RequestSendREPL::RequestSendREPL(const json& json_obj)
        : ReplMsg(json_obj["repl_msg_type"])
    {}

    void RequestSendREPL::apply(process::Process& process)
    {
        try
        {
            auto& client = dynamic_cast<client::Client&>(process);
            client.on_repl_send();
        }
        catch (const std::bad_cast&)
        {
            std::cerr << "Send message could not be applied\n";
        }
    }
} // namespace repl
