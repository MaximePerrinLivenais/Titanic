#pragma once

#include <memory>
#include <string>

#include "misc/json.hh"

class Server;

namespace rpc
{
    enum RPC_TYPE
    {
        CLIENT_REQUEST,
        CLIENT_RESPONSE,
        REQUEST_VOTE_RPC,
        REQUEST_VOTE_RESPONSE,
        APPEND_ENTRIES_RPC,
        APPEND_ENTRIES_RESPONSE
    };

    class RemoteProcedureCall;

    using shared_rpc = std::shared_ptr<RemoteProcedureCall>;

    class RemoteProcedureCall
    {
    public:
        explicit RemoteProcedureCall(const unsigned int term,
                                     const RPC_TYPE rpc_type);

        unsigned int get_term() const;
        RPC_TYPE get_rpc_type() const;

        const std::string serialize() const;
        static shared_rpc deserialize(const std::string &message);

        virtual void apply(Server &server) = 0;

        virtual ~RemoteProcedureCall() = default;

    private:
        virtual json serialize_json() const = 0;

        const unsigned int term;
        const RPC_TYPE rpc_type;
    };
} // namespace rpc
