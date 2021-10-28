#pragma once

#include <memory>
#include <string>

#include "message/message.hh"
#include "misc/json.hh"

using namespace message;

namespace process
{
    class Process;
}

namespace rpc
{
    enum RPC_TYPE
    {
        REQUEST_VOTE_RPC,
        REQUEST_VOTE_RESPONSE,
        APPEND_ENTRIES_RPC,
        APPEND_ENTRIES_RESPONSE
    };

    class RemoteProcedureCall;

    using shared_rpc = std::shared_ptr<RemoteProcedureCall>;

    class RemoteProcedureCall : public message::Message
    {
    public:
        explicit RemoteProcedureCall(const unsigned int term,
                                     const RPC_TYPE rpc_type);

        unsigned int get_term() const;
        RPC_TYPE get_rpc_type() const;

        static shared_rpc deserialize(const std::string& message);

        void apply_message(Process& process) final;

        // XXX: private to public
        virtual void apply(Server& server) = 0;

    protected:
        json serialize_json() const override;

        const unsigned int term;
        const RPC_TYPE rpc_type;
    };
} // namespace rpc
