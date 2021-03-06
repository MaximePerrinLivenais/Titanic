#include "rpc/rpc.hh"

#include <iostream>

#include "append-entries.hh"
#include "append-entries-response.hh"
#include "request-vote.hh"
#include "request-vote-response.hh"
#include "raft/server.hh"

using namespace message;

namespace rpc
{
    RemoteProcedureCall::RemoteProcedureCall(const unsigned int term,
                                             const RPC_TYPE rpc_type)
        : Message(MSG_TYPE::RPC_MESSAGE)
        , term(term)
        , rpc_type(rpc_type)
    {}

    void RemoteProcedureCall::apply(process::Process& process)
    {
        auto& server = dynamic_cast<raft::Server&>(process);
        server.on_rpc(*this);
    }

    json RemoteProcedureCall::serialize_json() const
    {
        json serialization = json();

        serialization["term"] = term;
        serialization["rpc_type"] = rpc_type;

        return serialization;
    }

    shared_rpc RemoteProcedureCall::deserialize(const std::string& message)
    {
        auto json_obj = json::parse(message);
        auto rpc_type = json_obj["rpc_type"].get<RPC_TYPE>();

        switch (rpc_type)
        {
        case REQUEST_VOTE_RPC:
            return std::make_shared<RequestVoteRPC>(json_obj);
        case REQUEST_VOTE_RESPONSE:
            return std::make_shared<RequestVoteResponse>(json_obj);
        case APPEND_ENTRIES_RPC:
            return std::make_shared<AppendEntriesRPC>(json_obj);
        case APPEND_ENTRIES_RESPONSE:
            return std::make_shared<AppendEntriesResponse>(json_obj);
        }

        throw std::invalid_argument("Not corresponding to existing RPCs");
    }

    unsigned int RemoteProcedureCall::get_term() const
    {
        return term;
    }

    RPC_TYPE RemoteProcedureCall::get_rpc_type() const
    {
        return rpc_type;
    }
} // namespace rpc
