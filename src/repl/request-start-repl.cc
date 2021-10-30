#include "request-start-repl.hh"

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
        auto& client = dynamic_cast<client::Client&>(process);
        client.on_repl_start();
    }

    json RequestStartREPL::serialize_json() const
    {
        json serialization = ReplMsg::serialize_json();

        return serialization;
    }

} // namespace repl
