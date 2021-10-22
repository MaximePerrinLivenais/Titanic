#include "server.hh"

#include <cstdlib>
#include <ctime>
#include <mpi.h>
#include <vector>
#include <fstream>

#include "exception/follower_exception.hh"
#include "utils/chrono/chrono.hh"
#include "utils/openmpi/mpi-wrapper.hh"

constexpr unsigned int MIN_TIMEOUT_MILLI = 150;
constexpr unsigned int MAX_TIMEOUT_MILLI = 300;

/*Server::Server()
    //: current_status(ServerStatus::FOLLOWER)
    //, vote_count(0)
// ... TODO
{}*/

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

    // XXX: what if you have 4 followers in with size = 10
    // I think we should check for votre_count * 2 + 1 >= ..
    return vote_count * 2 >= static_cast<unsigned int>(rank);
}

int Server::get_last_log_index()
{
    return log.size() - 1;
}

void Server::check_leader_rules()
{
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);
    int size = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);


    for (int idx = 1; idx < size; idx++)
    {
        if (idx == rank)
            continue;

        unsigned int last_log_index = get_last_log_index();
        // If last log index ≥ nextIndex for a follower
        if (last_log_index >= next_index[idx])
        {
            // send AppendEntries RPC with log entries starting at nextIndex
            std::vector<int> entries(log.begin() + next_index[idx],
                    log.end()); // FIXME: check if it is the correct range

            // XXX: last_log_term = log[last_log_index].term
            rpc::AppendEntriesRPC rpc(current_term, rank, last_log_index, 0, entries, 0);
            std::string message = rpc.serialize();

            // TODO: use enum RPC instead of dummy tag
            MPI_Send(message.data(), message.size(), MPI_CHAR, idx, 0, MPI_COMM_WORLD);
        }
    }


    // If there exists an N such that N > commitIndex, a majority of matchIndex[i] ≥ N,
    // and log[N].term == current
    // Term:set commitIndex = N (§5.3, §5.4).

}

void Server::update_commit_index()
{
    int size = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);
    bool updated = true;
    while (updated)
    {
        unsigned int n = commit_index + 1;

        int count = 0;
        for (int i = 0; i < size; i++)
        {
            if (match_index[i] >= n)
                count++;
        }

        // XXX: get real number of servers to check majority
        // check next_index.size()
        if (2 * count > size)
            commit_index = n;

        // If commit_index and n are equal it means we updated it in the loop
        updated = commit_index == n;
    }
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
    current_status = ServerStatus::LEADER;

    int size = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);

    // For each server, index of the next log entry to send to that server
    // (initialized to leader last log index + 1)
    next_index = std::vector<unsigned int>(log.size(), 0);

    // For each server, index of highest log entry known to be replicated on
    // server (initialized to 0, increases monotonically)
    match_index = std::vector<unsigned int>(size, 0);

    // XXX: Send empty AppendEntries RPC (heartbeat)
}

void Server::on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc)
{
    unsigned int last_log_index = get_last_log_index();
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);

    // 1. Reply false if term < currentTerm (§5.1)
    if (rpc.get_term() < current_term)
    {
        rpc::AppendEntriesResponse response(current_term, false, rank, last_log_index);
        std::string message = response.serialize();
        MPI_Send(message.data(), message.size(), MPI_CHAR, rpc.get_leader_id(),
                0, MPI_COMM_WORLD);
        return;
    }

    // 2. Reply false if log doesn’t contain an entry at prevLogIndex
    // whose term matches prevLogTerm (§5.3)
    // XXX: log[prev_log_index].term
    /*if (log[prev_log_index].term != prev_log_term)
    {
        rpc::AppendEntriesResponse response(current_term, false, rank);
        std::string message = response.serialize();
        MPI_Send(message.data(), message.size(), MPI_CHAR, rpc.get_leader_id(),
                0, MPI_COMM_WORLD);
    }*/

    // 3. If an existing entry conflicts with a new one 
    // (same index but different terms), delete the existing entry and all that
    // follow it (§5.3)


    // 4.  Append any new entries not already in the log
    // XXX: check that entries are not already in the log
    log.insert(log.end(), rpc.get_entries().begin(), rpc.get_entries().end());


    // 5. If leaderCommit > commitIndex, 
    // set commitIndex = min(leaderCommit, index of last new entry
    unsigned int last_entry = 0;//log.size() - 1;
    if (rpc.get_leader_commit_index() > commit_index)
        commit_index = std::min(rpc.get_leader_commit_index(), last_entry);

    rpc::AppendEntriesResponse response(current_term, true, rank, last_log_index);
    std::string message = response.serialize();
    MPI_Send(message.data(), message.size(), MPI_CHAR, rpc.get_leader_id(),
            0, MPI_COMM_WORLD);
}

void Server::on_append_entries_response(const rpc::AppendEntriesResponse& rpc)
{
    unsigned int follower_index = rpc.get_follower_index();
    if (rpc.get_success())
    {
        // XXX: last_log_index might changed between the AppendEntries and the response
        // we need the follower to send the index where he stops its log
        // and use rpc.last_log_index instead
        match_index[follower_index] = rpc.get_last_log_index();
        next_index[follower_index] = match_index[follower_index] + 1;
    }
    else
    {
        next_index[follower_index]--;
        // TODO: resend
    }
}





void Server::save_log() const
{
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::ofstream save_file("server_n" + std::to_string(rank) + ".log");

    for (const auto& entry : log)
        save_file << entry << "\n";
}

void Server::set_status(ServerStatus status)
{
    current_status = status;
}
