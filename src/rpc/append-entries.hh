#pragma once

#include <vector>

#include "rpc/log-entry.hh"
#include "rpc/rpc.hh"

namespace rpc
{
    class AppendEntriesRPC : public RemoteProcedureCall
    {
    public:
        explicit AppendEntriesRPC(const int term, const unsigned int leader_id,
                                  const unsigned int prev_log_index,
                                  const unsigned int prev_log_term,
                                  const std::vector<LogEntry> entries,
                                  const unsigned int leader_commit_index);

        explicit AppendEntriesRPC(const json& json_obj);

        void apply(raft::Server& server) final;

        unsigned int get_leader_id() const;
        int get_prev_log_index() const;
        int get_prev_log_term() const;
        const std::vector<LogEntry>& get_entries() const;
        int get_leader_commit_index() const;

    private:
        json serialize_json() const override;

        const unsigned int leader_id;
        const int prev_log_index;
        const int prev_log_term;

        const std::vector<LogEntry> entries;
        const int leader_commit_index;
    };
} // namespace rpc
