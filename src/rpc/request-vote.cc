#include "rpc/request-vote.hh"

using namespace rpc;

RequestVoteRPC::RequestVoteRPC(const int term,
                                const unsigned int candidate_id,
                                const unsigned int last_log_index,
                                const unsigned int last_log_term)
    : RemoteProcedureCall(term),
        candidate_id(candidate_id),
        last_log_index(last_log_index),
        last_log_term(last_log_term)
{}

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
