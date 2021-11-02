#include "message.hh"

#include <iostream>

#include "client/client-message.hh"
#include "repl/repl-message.hh"
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

    const std::string Message::serialize() const
    {
        json serialization = this->serialize_json();

        serialization["msg_type"] = msg_type;

        return serialization.dump();
    }

    shared_msg Message::deserialize(const std::string& message)
    {
        auto json_obj = json::parse(message);
        auto msg_type = json_obj["msg_type"].get<MSG_TYPE>();

        switch (msg_type)
        {
        case RPC_MESSAGE:
            return rpc::RemoteProcedureCall::deserialize(message);
        case REPL_MESSAGE:
            return repl::ReplMsg::deserialize(message);
        case CLIENT_MESSAGE:
            return client::ClientMsg::deserialize(message);
        }
        return nullptr;
    }

} // namespace message
