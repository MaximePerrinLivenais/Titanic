#include "server.hh"

#include <cstdlib>
#include <ctime>
#include <fstream>
// XXX Remove iostream include
#include <iostream>
#include <mpi.h>
#include <vector>

#include "exception/follower_exception.hh"
#include "raft/status.hh"
#include "rpc/rpc.hh"
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
    reset_timer();
}

void Server::run()
{
    // XXX: Debugging information
    std::cout << "[RUNNING] Server" << std::endl;
    while (true)
    {
        auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        while (query_str_opt.has_value())
        {
            // XXX: Debugging information
            // std::cout << "Receive: " << query_str_opt.value() << std::endl;

            reset_timer();

            auto query =
                rpc::RemoteProcedureCall::deserialize(query_str_opt.value());

            apply_query(query);

            query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        }

        if (current_status == ServerStatus::LEADER)
            apply_leader_rules();
        else
            apply_follower_and_candidate_rules();
    }
}

void Server::save_log() const
{
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);

    std::ofstream save_file("server_n" + std::to_string(rank) + ".log");

    for (const auto& entry : log)
        save_file << entry.get_command() << "\n";
}

/* ------------ Server reactions functions according to RPC type ------------ */

void Server::on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc)
{
    if (current_status == ServerStatus::CANDIDATE
        && rpc.get_term() >= current_term)
    {
        convert_to_follower();
    }

    unsigned int last_log_index = get_last_log_index();
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);

    // 1. Reply false if term < currentTerm (§5.1)
    if (rpc.get_term() < current_term)
    {
        rpc::AppendEntriesResponse response(current_term, false, rank,
                                            last_log_index);
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
    int last_entry = get_last_log_index();
    if (rpc.get_leader_commit_index() > commit_index)
        commit_index = std::min(rpc.get_leader_commit_index(), last_entry);

    rpc::AppendEntriesResponse response(current_term, true, rank,
                                        last_log_index);
    std::string message = response.serialize();
    MPI_Send(message.data(), message.size(), MPI_CHAR, rpc.get_leader_id(), 0,
             MPI_COMM_WORLD);
}

void Server::on_append_entries_response(const rpc::AppendEntriesResponse& rpc)
{
    unsigned int follower_index = rpc.get_follower_index();
    if (rpc.get_success())
    {
        // XXX: last_log_index might changed between the AppendEntries and the
        // response we need the follower to send the index where he stops its
        // log and use rpc.last_log_index instead
        match_index[follower_index] = rpc.get_last_log_index();
        next_index[follower_index] = match_index[follower_index] + 1;
    }
    else
    {
        next_index[follower_index]--;
        // TODO: resend
    }
}

void Server::on_request_vote_rpc(const rpc::RequestVoteRPC& rpc)
{
    if (current_status == ServerStatus::LEADER)
        return; // FIXME: A faire plus tard

    // 1. Reply false if term < currentTerm (§5.1)
    if (current_status == ServerStatus::CANDIDATE
        || rpc.get_term() < current_term)
    {
        // It already voted for candidate_id
        // Send RequestVoteResponse with granted_vote = false
        auto response = rpc::RequestVoteResponse(current_term, false);
        auto serialized_response = response.serialize();

        MPI_Send(serialized_response.c_str(), serialized_response.length(),
                 MPI_CHAR, rpc.get_candidate_id(), 0, MPI_COMM_WORLD);

        return;
    }

    // 2. If votedFor is null or candidateId, and candidate’s log is at
    // least as up-to-date as receiver’s log, grant vote (§5.2, §5.4)
    // XXX: Check log indexes
    voted_for = voted_for > 0 ? voted_for : rpc.get_candidate_id();

    auto response = rpc::RequestVoteResponse(current_term, voted_for > 0);
    auto serialized_response = response.serialize();

    MPI_Send(serialized_response.c_str(), serialized_response.length(),
             MPI_CHAR, rpc.get_candidate_id(), 0, MPI_COMM_WORLD);
}

void Server::on_request_vote_response(const rpc::RequestVoteResponse& rpc)
{
    if (current_status == ServerStatus::CANDIDATE)
    {
        vote_count += rpc.get_vote_granted();
        if (check_majority())
            convert_to_leader();
    }
}

/* ------------------------------ Server rules ------------------------------ */

void Server::apply_rules()
{
    // If commitIndex > lastApplied: increment lastApplied, apply
    // log[lastApplied] to state machine (§5.3)
    if (commit_index > last_applied)
    {
        last_applied++;
        // XXX: Apply it
    }
}

void Server::apply_follower_and_candidate_rules()
{
    if (chrono::get_time_milliseconds() - begin >= election_timeout)
        handle_election_timeout();
}

void Server::apply_leader_rules()
{
    if (chrono::get_time_milliseconds() - begin >= heartbeat_time)
    {
        begin = chrono::get_time_milliseconds();
        leader_heartbeat();
        return;
    }

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
            std::vector<rpc::LogEntry> entries(
                log.begin() + next_index[idx],
                log.end()); // FIXME: check if it is the correct range

            // XXX: last_log_term = log[last_log_index].term
            rpc::AppendEntriesRPC rpc(current_term, rank, last_log_index, 0,
                                      entries, 0);
            std::string message = rpc.serialize();

            // TODO: use enum RPC instead of dummy tag
            MPI_Send(message.data(), message.size(), MPI_CHAR, idx, 0,
                     MPI_COMM_WORLD);
        }
    }

    // If there exists an N such that N > commitIndex, a majority of
    // matchIndex[i] ≥ N, and log[N].term == current Term:set commitIndex = N
    // (§5.3, §5.4).
}

/* ---------------------- Useful auxialiary functions ---------------------- */

void Server::apply_query(rpc::shared_rpc query)
{
    if (query->get_term() > current_term)
    {
        current_term = query->get_term();
        convert_to_follower();
    }

    query->apply(*this);
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
        updated = commit_index == (int)n;
    }
}

void Server::handle_election_timeout()
{
    // XXX: Debugging information
    std::cout << "[ELECTION] Starting an election" << std::endl;

    reset_timer();

    current_status = ServerStatus::CANDIDATE;

    current_term += 1;

    // It votes for itself
    voted_for = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);
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

void Server::reset_timer()
{
    srand(time(0) + mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD));

    begin = chrono::get_time_milliseconds();

    auto range = MAX_TIMEOUT_MILLI - MIN_TIMEOUT_MILLI;
    election_timeout = std::rand() % (range) + MIN_TIMEOUT_MILLI;
}

bool Server::check_majority()
{
    int size = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);

    return vote_count * 2 >= static_cast<unsigned int>(size);
}

/* -------------------------------- Messages -------------------------------- */

void Server::broadcast_request_vote()
{
    auto rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);

    auto request_vote = rpc::RequestVoteRPC(current_term,
                                            rank,
                                            get_last_log_index(),
                                            get_last_log_term());

    mpi::MPI_Broadcast(request_vote.serialize(), 0, MPI_COMM_WORLD);
}

void Server::leader_heartbeat()
{
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);
    int prev_log_index = get_last_log_index();
    int prev_log_term = get_prev_log_term();

    auto empty_append_entry =
        rpc::AppendEntriesRPC(current_term, rank, prev_log_index, prev_log_term,
                              std::vector<rpc::LogEntry>(), commit_index);

    auto serialization = empty_append_entry.serialize();

    mpi::MPI_Broadcast(serialization, 0, MPI_COMM_WORLD);
}

/* ---------------------- Status conversion functions ---------------------- */

void Server::convert_to_follower()
{
    current_status = ServerStatus::FOLLOWER;
    voted_for = 0;

    // Reset timeout
    reset_timer();
}

void Server::convert_to_leader()
{
    current_status = ServerStatus::LEADER;
    voted_for = 0;

    int size = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);

    // For each server, index of the next log entry to send to that server
    // (initialized to leader last log index + 1)
    next_index = std::vector<unsigned int>(size, log.size());

    // For each server, index of highest log entry known to be replicated on
    // server (initialized to 0, increases monotonically)
    match_index = std::vector<unsigned int>(size, 0);

    // XXX: Debugging information
    std::cout << "[LEADER] term: " << current_term << std::endl;

    // Send empty AppendEntries RPC (heartbeat)
    leader_heartbeat();
}

/* ----------------------------- Useful getters ----------------------------- */

int Server::get_last_log_index()
{
    return log.size() - 1;
}

int Server::get_last_log_term()
{
    return log.empty() ? -1 : log.back().get_term();
}

int Server::get_prev_log_index()
{
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);

    return next_index[rank] - 1;
}

int Server::get_prev_log_term()
{
    int index = get_prev_log_index();

    // XXX: Decide if we want to return 0 as first term or -1
    return index >= 0 ? log[index].get_term() : -1;
}

// XXX: For testing purpose
void Server::set_status(const ServerStatus& server_status)
{
    current_status = server_status;
}
