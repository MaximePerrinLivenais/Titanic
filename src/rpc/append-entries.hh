#pragma once

#include <vector>

#include "rpc/rpc.hh"

namespace rpc
{
    class AppendEntriesRPC : public RemoteProcedureCall
    {
    public:
        explicit AppendEntriesRPC(const int term, const unsigned int leader_id,
                                  const unsigned int prev_log_index,
                                  const unsigned int prev_log_term,
                                  const std::vector<int> entries,
                                  const unsigned int leader_commit_index);

        explicit AppendEntriesRPC(const json& json_obj);

        unsigned int get_leader_id() const;
        unsigned int get_prev_log_index() const;
        unsigned int get_prev_log_term() const;
        const std::vector<int> &get_entries() const;
        unsigned int get_leader_commit_index() const;

        void apply(Server &server);

    private:
        json serialize_json() const;

        const unsigned int leader_id;
        const unsigned int prev_log_index;
        const unsigned int prev_log_term;
        const std::vector<int> entries;
        const unsigned int leader_commit_index;
    };
} // namespace rpc
