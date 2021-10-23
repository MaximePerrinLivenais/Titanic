#pragma once

#include "rpc/rpc.hh"

namespace client
{
    class ClientResponse : public rpc::RemoteProcedureCall
    {
    public:
        explicit ClientResponse(int result, bool success, unsigned int leader_id);
        explicit ClientResponse(const json& json_obj);

        static ClientResponse not_a_leader_response(unsigned int leader_id);

        void apply(Server &server);

    private:
        json serialize_json() const;

        const int result;
        const bool success;
        const unsigned int leader_id;
    };
}
