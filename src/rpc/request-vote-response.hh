#pragma once

#include "rpc.hh"

namespace rpc
{
    class RequestVoteResponse : public RemoteProcedureCall
    {
    public:
        explicit RequestVoteResponse(const int term, const bool vote_granted);

        explicit RequestVoteResponse(const json& json_obj);

        void apply(Server &server);
        bool get_vote_granted();

    private:
        json serialize_json() const;

        bool vote_granted;
    };
} // namespace rpc