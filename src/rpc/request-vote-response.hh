#pragma once

#include "rpc.hh"

namespace rpc
{
    class RequestVoteResponse : public RemoteProcedureCall
    {
    public:
        explicit RequestVoteResponse(const int term, const bool vote_granted);

        void apply(Server &server);
        bool get_vote_granted();

    private:
        bool vote_granted;
    };
} // namespace rpc
