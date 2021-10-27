#include "request-vote-response.hh"

#include "raft/server.hh"
#include "rpc/rpc.hh"

using namespace rpc;

RequestVoteResponse::RequestVoteResponse(const int term,
                                         const bool vote_granted)
    : RemoteProcedureCall(term, RPC_TYPE::REQUEST_VOTE_RESPONSE)
    , vote_granted(vote_granted)
{}

RequestVoteResponse::RequestVoteResponse(const json& json_obj)
    : RequestVoteResponse(json_obj["term"], json_obj["vote_granted"])
{}

void RequestVoteResponse::apply(Server& server)
{
    server.on_request_vote_response(*this);
}

bool RequestVoteResponse::get_vote_granted() const
{
    return vote_granted;
}

json RequestVoteResponse::serialize_json() const
{
    json serialization = RemoteProcedureCall::serialize_json();

    serialization["vote_granted"] = vote_granted;
    serialization["term"] = get_term();

    return serialization;
}
