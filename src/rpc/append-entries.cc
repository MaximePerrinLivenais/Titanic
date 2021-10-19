#include "rpc/append-entries.hh"

using namespace rpc;

AppendEntriesRPC::AppendEntriesRPC(const int term,
                                    const unsigned int leader_id,
                                    const unsigned int prev_log_index,
                                    const unsigned int prev_log_term,
                                    const std::vector<int> entries,
                                    const unsigned int leader_commit_index)
    : RemoteProcedureCall(term),
        leader_id(leader_id),
        prev_log_index(prev_log_index),
        prev_log_term(prev_log_term),
        entries(entries),
        leader_commit_index(leader_commit_index)
{}

unsigned int AppendEntriesRPC::get_leader_id() const
{
    return leader_id;
}

unsigned int AppendEntriesRPC::get_prev_log_index() const
{
    return prev_log_index;
}

unsigned int AppendEntriesRPC::get_prev_log_term() const
{
    return prev_log_term;
}

const std::vector<int>& AppendEntriesRPC::get_entries() const
{
    return entries;
}

unsigned int AppendEntriesRPC::get_leader_commit_index() const
{
    return leader_commit_index;
}
