#include "request-vote.hh"

#include "raft/server.hh"

namespace rpc
{
    RequestVoteRPC::RequestVoteRPC(const int term,
                                    const unsigned int candidate_id,
                                    const unsigned int last_log_index,
                                    const unsigned int last_log_term)
        : RemoteProcedureCall(term, REQUEST_VOTE_RPC),
            candidate_id(candidate_id),
            last_log_index(last_log_index),
            last_log_term(last_log_term)
    {}

    RequestVoteRPC::RequestVoteRPC(const json& json_obj)
        : RequestVoteRPC(json_obj["term"],
                            json_obj["candidate_id"],
                            json_obj["last_log_index"],
                            json_obj["last_log_term"])
    {}

    json RequestVoteRPC::serialize_json() const
    {
        json serialization = json();

        serialization["candidate_id"] = candidate_id;
        serialization["last_log_index"] = last_log_index;
        serialization["last_log_term"] = last_log_term;

        return serialization;
    }

    unsigned int RequestVoteRPC::get_candidate_id() const
    {
        return candidate_id;
    }

    unsigned int RequestVoteRPC::get_last_log_index() const
    {
        return last_log_index;
    }

    unsigned int RequestVoteRPC::get_last_log_term() const
    {
        return last_log_term;
    }


    void RequestVoteRPC::apply(Server &server)
    {
        if (server.get_status() == ServerStatus::CANDIDATE)
            server.count_vote(vote_granted);
    }
}

