#include "message.hh"

#include "rpc/rpc.hh"

namespace message
{
    Message::Message(const MSG_TYPE msg_type)
        : msg_type(msg_type)
    {}

    MSG_TYPE Message::get_msg_type() const
    {
        return msg_type;
    }

    shared_msg deserialize(const std::string& message)
    {
        auto json_obj = json::parse(message);
        auto msg_type = json_obj["msg_type"].get<MSG_TYPE>();

        switch (msg_type)
        {
        case RPC_MESSAGE:
            // TODO Not the best thing to do since we are deserializing two
            // times Maybe give the object `json` to deserialize.
            return rpc::RemoteProcedureCall::deserialize(message);
        case REPL_MESSAGE:
            abort(); // FIXME
        }
    }

} // namespace message
