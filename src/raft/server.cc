#include "server.hh"

#include <cstdlib>
#include <ctime>
#include <vector>

#include "exception/convert_to_follower.hh"
#include "utils/chrono/chrono.hh"

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

void Server::handle_election_timeout()
{
    set_election_timeout();

    current_term += 1;
    current_status = ServerStatus::CANDIDATE;
    vote_count = 0;

    // XXX: Broadcast for requesting vote RPC

    // Retrieve all messages in a vector
    auto queries = std::vector<rpc::RemoteProcedureCall>();

    // Check queries
    //
    // for each query
    // - if Request vote RPC
    //     count vote with vote_granted
    // - if Apprend entries RPC
    //     if its term is >= of our current term, then it becomes a follower.

    try
    {
        apply_queries(queries);
    }
    catch (const ConvertToFollower &)
    {
        convert_to_follower();
        return;
    }

    // check if vote_count has the majority of node
    //     - convert to leader
    // check if election_timeout is over
    //     - handle_election_timeout once again
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
    // TODO
}
