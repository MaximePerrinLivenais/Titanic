#include "rpc/append-entries.hh"

namespace rpc
{
    AppendEntriesRPC::AppendEntriesRPC(const int term,
                                        const unsigned int leader_id,
                                        const unsigned int prev_log_index,
                                        const unsigned int prev_log_term,
                                        const std::vector<int> entries,
                                        const unsigned int leader_commit_index)
        : RemoteProcedureCall(term, APPEND_ENTRIES_RPC),
            leader_id(leader_id),
            prev_log_index(prev_log_index),
            prev_log_term(prev_log_term),
            entries(entries),
            leader_commit_index(leader_commit_index)
    {}

    AppendEntriesRPC::AppendEntriesRPC(const json& json_obj)
        : AppendEntriesRPC(json_obj["term"],
                            json_obj["leader_id"],
                            json_obj["prev_log_index"],
                            json_obj["prev_log_term"],
                            std::vector<int>(0),
                            json_obj["leader_commit_index"])
    {}

    json AppendEntriesRPC::serialize_json() const
    {
        json serialization = json();

        serialization["leader_id"] = leader_id;
        serialization["prev_log_index"] = prev_log_index;
        serialization["prev_log_term"] = prev_log_term;
        // FIXME serialize entries
        serialization["leader_commit_index"] = leader_commit_index;

        return serialization;
    }

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
}
