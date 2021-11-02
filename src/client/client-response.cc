#include <iostream>
#include "client-response.hh"

#include "client/client.hh"
#include "raft/server.hh"

namespace client
{
    ClientResponse::ClientResponse(int result, bool success,
                                   unsigned int leader_id)
        : ClientMsg(CLIENT_MSG_TYPE::CLIENT_RESPONSE)
        , result(result)
        , success(success)
        , leader_id(leader_id)
    {}

    ClientResponse::ClientResponse(const json& json_obj)
        : ClientResponse(json_obj["result"], json_obj["success"],
                         json_obj["leader_id"])
    {}

    void ClientResponse::apply(process::Process& process)
    {
        try
        {
            auto& client = dynamic_cast<Client&>(process);
            client.on_client_response(*this);
        }
        catch(const std::bad_alloc&)
        {
            std::cout << "Could not apply client response\n";
        }
    }

    json ClientResponse::serialize_json() const
    {
        json serialization = ClientMsg::serialize_json();

        serialization["result"] = result;
        serialization["success"] = success;
        serialization["leader_id"] = leader_id;

        return serialization;
    }

    ClientResponse ClientResponse::not_a_leader_response(unsigned int leader_id)
    {
        return ClientResponse(0, false, leader_id);
    }

    bool ClientResponse::is_success() const
    {
        return success;
    }

    unsigned int ClientResponse::get_leader_id() const
    {
        return leader_id;
    }
} // namespace client
