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
    , current_term(0)
    , voted_for(0)
    , commit_index(0)
    , last_applied(0)
{
    set_election_timeout();
}

ServerStatus Server::get_status()
{
    return current_status;
}

unsigned int Server::get_term()
{
    return current_term;
}

unsigned int Server::get_voted_for()
{
    return voted_for;
}

void Server::set_voted_for(const unsigned int voted_for)
{
    this->voted_for = voted_for;
}

void Server::count_vote(const bool vote_granted)
{
    vote_count += vote_granted;
}

void Server::set_election_timeout()
{
    srand(time(0) + get_rank());

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

void Server::broadcast_request_vote()
{
    auto rank = get_rank();

    auto last_log_index = -1;
    auto last_log_term = -1;
    if (!log.empty())
    {
        last_log_index = log.back().term;
        last_log_term = log.size() - 1;
    }

    auto request_vote =
        rpc::RequestVoteRPC(current_term, rank, last_log_index, last_log_term);

    mpi::MPI_Broadcast(request_vote.serialize(), 0, MPI_COMM_WORLD);
}

void Server::run()
{
    std::cout << "[RUNNING] Server" << std::endl;
    while (true)
    {
        if (chrono::get_time_milliseconds() - begin >= election_timeout)
            handle_election_timeout();

        auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        if (query_str_opt.has_value())
        {
            std::cout << "Receive: " << query_str_opt.value() << std::endl;

            set_election_timeout();

            auto query =
                rpc::RemoteProcedureCall::deserialize(query_str_opt.value());
            query->apply(*this);
        }
    }
}

void Server::handle_election_timeout()
{
    std::cout << "[ELECTION] Starting an election" << std::endl;

    set_election_timeout();

    current_status = ServerStatus::CANDIDATE;

    current_term += 1;

    // It votes for itself
    voted_for = get_rank();
    vote_count = 1;

    // Broadcast request vote RPC
    broadcast_request_vote();

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
    set_election_timeout();
}

void Server::convert_to_leader()
{
    // TODO
    current_status = ServerStatus::LEADER;
    voted_for = 0;

    std::cout << "[LEADER] term: " << get_term() << std::endl;

    // XXX: Send empty AppendEntries RPC (heartbeat)
}
