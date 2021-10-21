#include "request-vote.hh"

#include <mpi.h>

#include "raft/server.hh"
#include "rpc/request-vote-response.hh"

namespace rpc
{
    RequestVoteRPC::RequestVoteRPC(const unsigned term,
                                   const unsigned int candidate_id,
                                   const int last_log_index,
                                   const int last_log_term)
        : RemoteProcedureCall(term, RPC_TYPE::REQUEST_VOTE_RPC)
        , candidate_id(candidate_id)
        , last_log_index(last_log_index)
        , last_log_term(last_log_term)
    {}

    RequestVoteRPC::RequestVoteRPC(const json &json_obj)
        : RequestVoteRPC(json_obj["term"], json_obj["candidate_id"],
                         json_obj["last_log_index"], json_obj["last_log_term"])
    {}

    json RequestVoteRPC::serialize_json() const
    {
        json serialization = json();

        serialization["candidate_id"] = candidate_id;
        serialization["last_log_index"] = last_log_index;
        serialization["last_log_term"] = last_log_term;

        return serialization;
    }

    unsigned int RequestVoteRPC::get_candidate_id() const
    {
        return candidate_id;
    }

    int RequestVoteRPC::get_last_log_index() const
    {
        return last_log_index;
    }

    int RequestVoteRPC::get_last_log_term() const
    {
        return last_log_term;
    }

    void RequestVoteRPC::apply(Server &server)
    {
        if (server.get_status() == ServerStatus::CANDIDATE)
        {
            // It already voted for candate_id
            // Send RequestVoteResponse with granted_vote = false
            auto response = RequestVoteResponse(server.get_term(), false);
            auto serialized_response = response.serialize();

            MPI_Send(serialized_response.c_str(), serialized_response.size(),
                     MPI_CHAR, candidate_id, 0, MPI_COMM_WORLD);
        }
        else
        {
            // XXX: We have to compare the last_log_index and last_log_term
            auto response = RequestVoteResponse(server.get_term(), true);
            auto serialized_response = response.serialize();

            MPI_Send(serialized_response.c_str(), serialized_response.size(),
                     MPI_CHAR, candidate_id, 0, MPI_COMM_WORLD);
        }
    }
} // namespace rpc
