#pragma once

#include "rpc.hh"

namespace rpc
{
    class RequestVoteRPC : public RemoteProcedureCall
    {
    public:
        explicit RequestVoteRPC(const unsigned term,
                                const unsigned int candidate_id,
                                const int last_log_index,
                                const int last_log_term);

        explicit RequestVoteRPC(const json& json_obj);

        void apply(raft::Server& server) override;

        unsigned int get_candidate_id() const;
        int get_last_log_index() const;
        int get_last_log_term() const;

    private:
        json serialize_json() const override;

        const unsigned int candidate_id;
        const int last_log_index;
        const int last_log_term;
    };
} // namespace rpc
