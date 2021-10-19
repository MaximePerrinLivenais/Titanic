#pragma once

#include "rpc/rpc.hh"

#include <vector>

namespace rpc
{
    class AppendEntriesRPC : public RemoteProcedureCall
    {
        public:
            explicit AppendEntriesRPC(const int term,
                                        const unsigned int leader_id,
                                        const unsigned int prev_log_index,
                                        const unsigned int prev_log_term,
                                        const std::vector<int> entries,
                                        const unsigned int leader_commit_index);

            unsigned int get_leader_id() const;
            unsigned int get_prev_log_index() const;
            unsigned int get_prev_log_term() const;
            const std::vector<int>& get_entries() const;
            unsigned int get_leader_commit_index() const;

        private:
            const unsigned int leader_id;
            const unsigned int prev_log_index;
            const unsigned int prev_log_term;
            const std::vector<int> entries;
            const unsigned int leader_commit_index;
    };
}
