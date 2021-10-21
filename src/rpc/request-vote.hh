#pragma once

#include "rpc/rpc.hh"

namespace rpc
{
    class RequestVoteRPC : public RemoteProcedureCall
    {
        public:
            explicit RequestVoteRPC(const int term,
                                    const unsigned int candidate_id,
                                    const unsigned int last_log_index,
                                    const unsigned int last_log_term);

            explicit RequestVoteRPC(const json& json_obj);

            unsigned int get_candidate_id() const;
            unsigned int get_last_log_index() const;
            unsigned int get_last_log_term() const;

        private:
            json serialize_json() const;

            const unsigned int candidate_id;
            const unsigned int last_log_index;
            const unsigned int last_log_term;
    };
}
