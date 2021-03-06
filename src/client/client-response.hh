#pragma once

#include "client-message.hh"

namespace client
{
    class ClientResponse : public ClientMsg
    {
    public:
        explicit ClientResponse(int result, bool success,
                                unsigned int leader_id);
        explicit ClientResponse(const json& json_obj);

        void apply(process::Process& process) final;

        bool is_success() const;
        unsigned int get_leader_id() const;

        static ClientResponse not_a_leader_response(unsigned int leader_id);

    private:
        json serialize_json() const;

        const int result;
        const bool success;
        const unsigned int leader_id;
    };
} // namespace client
