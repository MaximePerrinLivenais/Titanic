#include "request-vote-response.hh"

#include "raft/server.hh"
#include "rpc/rpc.hh"

using namespace rpc;

RequestVoteResponse::RequestVoteResponse(const int term,
                                         const bool vote_granted)
    : RemoteProcedureCall(term)
    , vote_granted(vote_granted)
{}

void RequestVoteResponse::apply(Server &server)
{
    if (server.get_status() == ServerStatus::CANDIDATE)
        server.count_vote(vote_granted);
}

bool RequestVoteResponse::get_vote_granted()
{
    return vote_granted;
}
