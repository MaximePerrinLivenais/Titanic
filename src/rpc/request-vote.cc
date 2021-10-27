#include "request-vote.hh"

#include <mpi.h>

#include "raft/server.hh"
#include "raft/status.hh"
#include "rpc/request-vote-response.hh"

namespace rpc
{
    RequestVoteRPC::RequestVoteRPC(const unsigned term,
                                   const unsigned int candidate_id,
                                   const int last_log_index,
                                   const int last_log_term)
        : RemoteProcedureCall(term, RPC_TYPE::REQUEST_VOTE_RPC)
        , candidate_id(candidate_id)
        , last_log_index(last_log_index)
        , last_log_term(last_log_term)
    {}

    RequestVoteRPC::RequestVoteRPC(const json& json_obj)
        : RequestVoteRPC(json_obj["term"], json_obj["candidate_id"],
                         json_obj["last_log_index"], json_obj["last_log_term"])
    {}

    json RequestVoteRPC::serialize_json() const
    {
        json serialization = RemoteProcedureCall::serialize_json();

        serialization["candidate_id"] = candidate_id;
        serialization["last_log_index"] = last_log_index;
        serialization["last_log_term"] = last_log_term;

        return serialization;
    }

    unsigned int RequestVoteRPC::get_candidate_id() const
    {
        return candidate_id;
    }

    int RequestVoteRPC::get_last_log_index() const
    {
        return last_log_index;
    }

    int RequestVoteRPC::get_last_log_term() const
    {
        return last_log_term;
    }

    void RequestVoteRPC::apply(Server& server)
    {
        server.on_request_vote_rpc(*this);
    }
} // namespace rpc
