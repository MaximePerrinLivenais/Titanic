#include "client-message.hh"

#include <iostream>

#include "client-request.hh"
#include "client-response.hh"
#include "raft/server.hh"

namespace client
{
    ClientMsg::ClientMsg(const CLIENT_MSG_TYPE client_type)
        : Message(message::MSG_TYPE::CLIENT_MESSAGE)
        , client_msg_type(client_type)
    {}

    json ClientMsg::serialize_json() const
    {
        json serialization = json();

        serialization["client_msg_type"] = client_msg_type;

        return serialization;
    }

    shared_client_msg ClientMsg::deserialize(const std::string& message)
    {
        auto json_obj = json::parse(message);
        auto client_msg_type =
            json_obj["client_msg_type"].get<CLIENT_MSG_TYPE>();

        switch (client_msg_type)
        {
        case CLIENT_REQUEST:
            return std::make_shared<ClientRequest>(json_obj);
        case CLIENT_RESPONSE:
            return std::make_shared<ClientResponse>(json_obj);
        }

        throw std::invalid_argument(
            "Not corresponding to existing client message");
    }

    CLIENT_MSG_TYPE ClientMsg::get_client_msg_type() const
    {
        return client_msg_type;
    }

} // namespace client
