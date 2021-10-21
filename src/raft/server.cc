#include "server.hh"

#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <vector>

#include "exception/follower_exception.hh"
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

    auto range = MAX_TIMEOUT_MILLI - MIN_TIMEOUT_MILLI;
    election_timeout = std::rand() % (range) + MIN_TIMEOUT_MILLI;
}

bool Server::check_majority()
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    return vote_count * 2 >= static_cast<unsigned int>(rank);
}

void Server::handle_election_timeout()
{
    set_election_timeout();

    auto begin = chrono::get_time_milliseconds();

    current_term += 1;
    current_status = ServerStatus::CANDIDATE;

    // It votes for itself
    vote_count = 1;

    // XXX: Broadcast for requesting vote RPC
    std::string message = "test";
    mpi::MPI_Broadcast(message, 0, MPI_COMM_WORLD);

    // Retrieve all messages in a vector
    auto queries = std::vector<rpc::RemoteProcedureCall>();
    auto buffer = mpi::MPI_Listen(MPI_COMM_WORLD);

    while (buffer.has_value())
    {
        // XXX: Deserialize the message to query
        // XXX: Add to queries
        buffer = mpi::MPI_Listen(MPI_COMM_WORLD);
    }

    // Check queries
    //
    // for each query
    // - if Request vote RPC
    //     count vote with vote_granted
    // - if Apprend entries RPC
    //     if its term is >= of our current term, then it becomes a
    //     follower.

    try
    {
        apply_queries(queries);
    }
    catch (const FollowerException &)
    {
        convert_to_follower();
        return;
    }

    // XXX: if vote_count has the majority of node
    //     - convert to leader
    if (check_majority())
    {
        convert_to_leader();
        return;
    }

    // check if election_timeout is over
    //     - handle_election_timeout once again
    if (chrono::get_time_milliseconds() - begin >= election_timeout)
        handle_election_timeout();
}

void Server::apply_queries(std::vector<rpc::RemoteProcedureCall> &queries)
{
    for (auto &query : queries)
    {
        query.apply(*this);
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
