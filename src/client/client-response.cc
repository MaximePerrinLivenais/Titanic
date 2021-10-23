#include "client-response.hh"
#include "raft/server.hh"

namespace client
{
    ClientResponse::ClientResponse(int result, bool success, unsigned int leader_id) :
        RemoteProcedureCall(-1, rpc::RPC_TYPE::CLIENT_RESPONSE), result(result),
        success(success), leader_id(leader_id)
    {}

    ClientResponse::ClientResponse(const json& json_obj) :
        ClientResponse(json_obj["result"], json_obj["success"], json_obj["leader_id"])
    {}

    ClientResponse ClientResponse::not_a_leader_response(unsigned int leader_id)
    {
        return ClientResponse(0, false, leader_id);
    }

    json ClientResponse::serialize_json() const
    {
        json serialization = json();

        serialization["result"] = result;
        serialization["success"] = success;
        serialization["leader_id"] = leader_id;

        return serialization;
    }

    void ClientResponse::apply(Server &server)
    {
        // XXX: this function should never be called because its only for the client
        (void)server;
    }
}
