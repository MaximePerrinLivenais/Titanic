#include "server.hh"

#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <vector>

#include "exception/follower_exception.hh"
#include "rpc/request-vote.hh"
#include "utils/chrono/chrono.hh"
#include "utils/openmpi/mpi-wrapper.hh"

constexpr unsigned int MIN_TIMEOUT_MILLI = 150;
constexpr unsigned int MAX_TIMEOUT_MILLI = 300;

Server::Server()
    : current_status(ServerStatus::FOLLOWER)
    , vote_count(0)
// ... TODO
{}

ServerStatus Server::get_status()
{
    return current_status;
}

unsigned int Server::get_term()
{
    return current_term;
}

void Server::count_vote(const bool vote_granted)
{
    vote_count += vote_granted;
}

void Server::set_election_timeout()
{
    srand(time(0));

    begin = chrono::get_time_milliseconds();

    auto range = MAX_TIMEOUT_MILLI - MIN_TIMEOUT_MILLI;
    election_timeout = std::rand() % (range) + MIN_TIMEOUT_MILLI;
}

bool Server::check_majority()
{
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    return vote_count * 2 >= static_cast<unsigned int>(size);
}

unsigned int Server::get_rank()
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    return rank;
}

void Server::broadcast_rpc()
{
    auto rank = get_rank();

    auto last_log_index = log.size() - 1;
    auto last_log_term = -1;
    if (!log.empty())
        last_log_index = log.back().term;

    auto request_vote =
        rpc::RequestVoteRPC(current_term, rank, last_log_index, last_log_term);

    mpi::MPI_Broadcast(request_vote.serialize(), 0, MPI_COMM_WORLD);
}

void Server::run()
{
    while (true)
    {
        if (chrono::get_time_milliseconds() - begin >= election_timeout)
            handle_election_timeout();

        auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        if (query_str_opt.has_value())
        {
            set_election_timeout();

            auto query =
                rpc::RemoteProcedureCall::deserialize(query_str_opt.value());
            query->apply(*this);
        }
    }
}

void Server::handle_election_timeout()
{
    set_election_timeout();

    current_status = ServerStatus::CANDIDATE;

    current_term += 1;

    // It votes for itself
    vote_count = 1;

    // Broadcast for requesting vote RPC
    broadcast_rpc();

    // Check queries
    //
    // for each query
    // - if Request vote RPC
    //     count vote with vote_granted
    // - if Apprend entries RPC
    //     if its term is >= of our current term, then it becomes a
    //     follower.
    // if vote_count has the majority of node
    //     - convert to leader
    // check if election_timeout is over
    //     - handle_election_timeout once again
}

void Server::apply_queries(std::vector<rpc::shared_rpc> &queries)
{
    for (auto &query : queries)
    {
        query->apply(*this);
    }
}

void Server::convert_to_follower()
{
    current_status = ServerStatus::FOLLOWER;
    voted_for = 0;

    // XXX: Reset timeout
}

void Server::convert_to_leader()
{
    // TODO
    current_status = ServerStatus::LEADER;

    // XXX: Send empty AppendEntries RPC (heartbeat)
}
