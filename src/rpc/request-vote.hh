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

        unsigned int get_candidate_id() const;
        int get_last_log_index() const;
        int get_last_log_term() const;

        void apply(Server& server) override;

    private:
        json serialize_json() const override;

        const unsigned int candidate_id;
        const int last_log_index;
        const int last_log_term;
    };
} // namespace rpc
