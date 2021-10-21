#include "rpc/rpc.hh"

#include <iostream>

#include "append-entries.hh"
#include "request-vote.hh"

namespace rpc
{
    RemoteProcedureCall::RemoteProcedureCall(const int term, const RPC_TYPE rpc_type)
        : term(term), rpc_type(rpc_type)
    {}

    const std::string RemoteProcedureCall::serialize() const
    {
        json serialization = this->serialize_json();

        serialization["term"] = term;
        serialization["rpc_type"] = rpc_type;

        return serialization.dump(4);
    }

    shared_rpc RemoteProcedureCall::deserialize(const std::string& message)
    {
        auto json_obj = json::parse(message);
        auto rpc_type = json_obj["rpc_type"].get<RPC_TYPE>();

        std::cout << rpc_type << "\n";

        switch(rpc_type)
        {
            case REQUEST_VOTE_RPC:
                return std::make_shared<RequestVoteRPC>(json_obj);
            case APPEND_ENTRIES_RPC:
                return std::make_shared<AppendEntriesRPC>(json_obj);
        }

        throw std::invalid_argument("Not corresponding to existing RPCs");
    }

    int RemoteProcedureCall::get_term() const
    {
        return term;
    }

    RPC_TYPE RemoteProcedureCall::get_rpc_type() const
    {
        return rpc_type;
    }
}
