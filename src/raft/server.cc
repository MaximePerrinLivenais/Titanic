#include "server.hh"

#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
// XXX Remove iostream include
#include <iostream>
#include <mpi.h>
#include <vector>

#include "client/client-response.hh"
#include "exception/follower_exception.hh"
#include "raft/status.hh"
#include "rpc/rpc.hh"
#include "utils/chrono/chrono.hh"
#include "utils/openmpi/mpi-wrapper.hh"

constexpr unsigned int MIN_TIMEOUT_MILLI = 1500;
constexpr unsigned int MAX_TIMEOUT_MILLI = 3000;

Server::Server(int server_rank, int nb_servers)
    : server_rank(server_rank)
    , nb_servers(nb_servers)
    , current_status(ServerStatus::FOLLOWER)
    , vote_count(0)
    , current_term(0)
    , voted_for(0)
    , commit_index(-1)
    , last_applied(-1)
{
    reset_election_timeout();
}

void Server::run()
{
    // XXX: Debugging information
    // std::cout << "[RUNNING] Server" << std::endl;
    while (true)
    {
        // Maybe do it several time before each I/O function
         std::this_thread::sleep_for(std::chrono::milliseconds(latency));

        // Rules for Servers - Followers (§5.2):
        //      Respond to RPCs from candidates and leaders
        auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        while (query_str_opt.has_value())
        {
            // XXX: Debugging information
            // std::cout << "Receive: " << query_str_opt.value() << std::endl;

            auto query = message::Message::deserialize(query_str_opt.value());

            // TODO here we can deserialize direclty in message
            // auto query =
            // rpc::RemoteProcedureCall::deserialize(query_str_opt.value());


            // XXX
            // here call apply_message
            // In the case of rpc -> do the same as apply_query
            // In the case of repl -> apply directly
            //
            //
            // XXX: apply message for CRASHED server only if from REPL

            query->apply_message(*this);

            // std::cout << "My status is: " << current_status << std::endl;

            query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);

        }


        if (!is_alive())
            continue;

        save_log();

        apply_rules();
        if (current_status == ServerStatus::LEADER)
            apply_leader_rules();
        else if (current_status == ServerStatus::CANDIDATE
                 || current_status == ServerStatus::FOLLOWER)
            apply_follower_and_candidate_rules();
    }
}

void Server::save_log() const
{
    std::string filename = "server_logs/server_n" + std::to_string(server_rank) + ".log";

    MPI_File file;
    // TODO: Check if it is open

    MPI_File_open(MPI_COMM_SELF, filename.data(),
            MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &file);
    MPI_Status status;

    for (const auto& entry : log)
    {
        std::string command = entry.get_command() + "\n";
        MPI_File_write(file, command.data(), command.size(), MPI_CHAR, &status);
    }

    MPI_File_close(&file);
}

void Server::update_term(unsigned int term)
{
    if (term > current_term)
    {
        set_current_term(term);
        convert_to_follower();
    }
}

void Server::crash()
{
    alive = false;
}

/* ------------ Server reactions functions according to RPC type ------------ */

void Server::on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc)
{
    // Rules for Servers - Candidates (§5.2):
    //      If AppendEntries RPC received from new leader: convert to follower
    if (current_status == ServerStatus::CANDIDATE
        && rpc.get_term() >= current_term)
        convert_to_follower();

    // AppendEntries RPC - Receiver implementation

    // 1. Reply false if term < currentTerm (§5.1)
    if (rpc.get_term() < current_term)
    {
        auto response = std::make_shared<rpc::AppendEntriesResponse>(current_term,
                                                                        false,
                                                                        server_rank,
                                                                        get_last_log_index());
        mpi::MPI_Serialize_and_send(response, rpc.get_leader_id(), 0, MPI_COMM_WORLD);
        return;
    }

    if (current_status == ServerStatus::FOLLOWER)
        begin = chrono::get_time_milliseconds();

    // 2. Reply false if log doesn’t contain an entry at prevLogIndex whose term
    // matches prevLogTerm (§5.3)
    if (get_term_at_prev_log_index(rpc.get_prev_log_index())
        != rpc.get_prev_log_term())
    {
        auto response = std::make_shared<rpc::AppendEntriesResponse>(current_term,
                                                                        false,
                                                                        server_rank,
                                                                        get_last_log_index());
        mpi::MPI_Serialize_and_send(response, rpc.get_leader_id(), 0, MPI_COMM_WORLD);
        return;
    }

    // 3. If an existing entry conflicts with a new one (same index but
    // different terms), delete the existing entry and all that follow it (§5.3)
    // TODO

    // 4.  Append any new entries not already in the log
    if (rpc.get_entries().size())
    {
        for (const auto& entry : rpc.get_entries())
        {
            // XXX: we might do something smarter using Log Matching property
            if (std::find(log.begin(), log.end(), entry) == log.end())
            {
                std::cout << "Server n" << server_rank << " add to its log\n" << std::flush;
                log.emplace_back(entry);
            }
        }
        //log.insert(log.end(), rpc.get_entries().begin(), rpc.get_entries().end());
    }

    // 5. If leaderCommit > commitIndex,
    // set commitIndex = min(leaderCommit, index of last new entry)
    int last_entry_index = get_last_log_index();
    if (rpc.get_leader_commit_index() > commit_index)
        commit_index =
            std::min(rpc.get_leader_commit_index(), last_entry_index);

    auto response = std::make_shared<rpc::AppendEntriesResponse>(current_term,
                                                                    true,
                                                                    server_rank,
                                                                    last_entry_index);
    mpi::MPI_Serialize_and_send(response, rpc.get_leader_id(), 0, MPI_COMM_WORLD);
}

void Server::on_append_entries_response(const rpc::AppendEntriesResponse& rpc)
{
    unsigned int follower_index = rpc.get_follower_index();

    // Rules for Servers - Leaders:
    // If last log index ≥ nextIndex for a follower: send AppendEntries RPC
    // with log entries starting at nextIndex:
    //      • If successful: update nextIndex and matchIndex for follower (§5.3)
    //      • If AppendEntries fails because of log inconsistency: decrement
    //        nextIndex and retry (§5.3)
    if (rpc.get_success())
    {
        // last_log_index might changed between the AppendEntries and the
        // response we need the follower to send the index where he stops its
        // log and use rpc.last_log_index instead
        match_index[follower_index] = rpc.get_last_log_index();
        next_index[follower_index] = match_index[follower_index] + 1;
    }
    else
    {
        next_index[follower_index]--;
    }
}

void Server::on_request_vote_rpc(const rpc::RequestVoteRPC& rpc)
{
    if (current_status == ServerStatus::LEADER)
        return; // TODO: A faire plus tard

    // RequestVote RPC - Receiver implementation
    // 1. Reply false if term < currentTerm (§5.1)
    if (current_status == ServerStatus::CANDIDATE
        || rpc.get_term() < current_term)
    {
        auto response = std::make_shared<rpc::RequestVoteResponse>(current_term, false);
        mpi::MPI_Serialize_and_send(response, rpc.get_candidate_id(), 0, MPI_COMM_WORLD);
        return;
    }

    // 2. If votedFor is null or candidateId, and candidate’s log is at
    // least as up-to-date as receiver’s log, grant vote (§5.2, §5.4)
    // FIXME: Check log indexes
    if (!candidate_log_is_up_to_date(rpc.get_last_log_index(),
                                        rpc.get_last_log_term()))
    {
        auto response = std::make_shared<rpc::RequestVoteResponse>(current_term, false);
        mpi::MPI_Serialize_and_send(response, rpc.get_candidate_id(), 0, MPI_COMM_WORLD);
        return;
    }

    voted_for = voted_for > 0 ? voted_for : rpc.get_candidate_id();

    auto response = std::make_shared<rpc::RequestVoteResponse>(current_term, voted_for > 0);
    mpi::MPI_Serialize_and_send(response, rpc.get_candidate_id(), 0, MPI_COMM_WORLD);
}

void Server::on_request_vote_response(const rpc::RequestVoteResponse& rpc)
{
    if (current_status == ServerStatus::CANDIDATE)
    {
        vote_count += rpc.get_vote_granted();

        // Rules for Servers - Candidates (§5.2):
        //      If votes received from majority of servers: become leader
        if (check_majority())
            convert_to_leader();
    }
}

/* ------------------------------ Server rules ------------------------------ */

void Server::apply_rules()
{
    // Rules for Servers - All Servers:
    //      If commitIndex > lastApplied: increment lastApplied, apply
    //      log[lastApplied] to state machine (§5.3)
    //std::cout << "Commit index = " << commit_index << " vs Last applied = " << last_applied << "\n";
    if (commit_index > last_applied)
    {
        std::cout << "Apply log\n";
        last_applied++;
        // XXX: Apply it

        // Answer to client
        //
        if (current_status == ServerStatus::LEADER)
        {
            client::ClientResponse response(0, true, 0);
            std::string message = response.serialize();

            auto client_index =  log[last_applied].get_client_index();

            MPI_Send(message.data(), message.size(), MPI_CHAR, client_index,
                    0, MPI_COMM_WORLD);
        }
    }
}

void Server::apply_follower_and_candidate_rules()
{
    // Rules for Servers - Followers (§5.2):
    //      If election timeout elapses without receiving AppendEntries
    //      RPC from current leader or granting vote to candidate
    // Rules for Servers - Candidates (§5.2):
    //      If election timeout elapses: start new election
    //      FIXME: Not correctly dealed with for the moement
    if (chrono::get_time_milliseconds() - begin >= election_timeout)
        convert_to_candidate();
}

void Server::apply_leader_rules()
{
    // Rules for Servers - Leaders:
    //      Upon election: send initial empty AppendEntries RPCs (heartbeat)
    //      to each server; repeat during idle periods to prevent election
    //      timeouts (§5.2)
    if (chrono::get_time_milliseconds() - begin >= heartbeat_time)
    {
        begin = chrono::get_time_milliseconds();
        leader_heartbeat();

        /*auto log_entry =
            rpc::LogEntry(current_term,
                          "term = " + std::to_string(current_term)
                              + ", time = " + std::to_string(begin));
        log.push_back(log_entry);*/
        return;
    }

    // Rules for Servers - Leaders:
    //      If last log index ≥ nextIndex for a follower: send AppendEntries RPC
    //      with log entries starting at nextIndex
    for (int idx = 1; idx <= nb_servers; idx++)
    {
        if (idx == server_rank)
            continue;

        int last_log_index = get_last_log_index();
        int prev_log_index = get_prev_log_index(idx);
        int prev_log_term = get_prev_log_term(idx);

        if (last_log_index >= next_index[idx])
        {
            // FIXME: check if it is the correct range
            std::vector<rpc::LogEntry> entries(log.begin() + next_index[idx], log.end());

            auto rpc = std::make_shared<rpc::AppendEntriesRPC>(current_term,
                                                                server_rank,
                                                                prev_log_index,
                                                                prev_log_term,
                                                                entries,
                                                                commit_index);
            mpi::MPI_Serialize_and_send(rpc, idx, 0, MPI_COMM_WORLD);
        }
    }

    // Rules for Servers - Leaders:
    //      If there exists an N such that N > commitIndex, a majority of
    //      matchIndex[i] ≥ N, and log[N].term == currentTerm:
    //      set commitIndex = N (§5.3, §5.4).
    update_commit_index();
}

void Server::update_term(unsigned int term)
{
    if (term > current_term)
    {
        set_current_term(term);
        convert_to_follower();
    }
}

void Server::crash()
{
    alive = false;
}

void Server::change_speed(repl::ServerSpeed speed)
{
    // I prefer to set latency related to heartbeat_time
    // Maybe set the speed randomly at each loop
    if (speed == repl::LOW)
        latency = heartbeat_time * 0.8;
    else if (speed == repl::MEDIUM)
        latency = heartbeat_time * 0.4;
    else
        latency = 0;

    std::cout << "Speed set to " << latency << " for server " << server_rank <<"\n";
}

/* ---------------------- Useful auxialiary functions ---------------------- */

void Server::set_current_term(const int current_term)
{
    this->current_term = current_term;

    begin = chrono::get_time_milliseconds();
    reset_election_timeout();
}

/*void Server::apply_query(message::shared_msg query)
{
    // Rules for Servers - All Servers:
    //      If RPC request or response contains term T > currentTerm:
    //      set currentTerm = T, convert to follower (§5.1)
    if (get_status() == CRASHED)
        return;

    if (query->get_term() > current_term)
    {
        set_current_term(query->get_term());
        convert_to_follower();
    }

    query->apply(*this);
}*/

void Server::update_commit_index()
{
    bool updated = true;
    while (updated)
    {
        int n = commit_index + 1;

        int count = 0;
        for (int i = 1; i <= nb_servers; i++)
        {
            if (match_index[i] >= n)
                count++;
        }

        // XXX: get real number of servers to check majority
        // check next_index.size()
        if (2 * count > nb_servers)
            commit_index = n;

        // If commit_index and n are equal it means we updated it in the loop
        updated = commit_index == n;
    }
}

/// XXX: Rename it to convert_to_candidate ?
void Server::convert_to_candidate()
{
    // XXX: Debugging information
    std::cout << "[ELECTION] " << server_rank << " is Starting an election"
              << std::endl;

    // Rules for Servers - Candidates (§5.2):
    // On conversion to candidate, start election:
    //      • Increment currentTerm
    //      • Vote for self
    //      • Reset election timer
    //      • Send RequestVote RPCs to all other servers
    current_status = ServerStatus::CANDIDATE;

    set_current_term(current_term + 1);

    voted_for = server_rank;
    vote_count = 1;

    broadcast_request_vote();
}

void Server::reset_election_timeout()
{
    srand(time(0) + server_rank);

    auto range = MAX_TIMEOUT_MILLI - MIN_TIMEOUT_MILLI;
    election_timeout = std::rand() % (range) + MIN_TIMEOUT_MILLI;
}

bool Server::check_majority()
{
    // XXX: avoir unsigned and signed comparaison
    int vote = vote_count;
    return vote * 2 > nb_servers;
}

bool Server::candidate_log_is_up_to_date(int last_log_index, int last_log_term)
{
    if (last_log_term < get_last_log_term())
        return false;

    return last_log_term > get_last_log_term()
            || last_log_index >= get_last_log_index();
}

/* -------------------------------- Messages -------------------------------- */

void Server::broadcast_request_vote()
{
    auto request_vote = rpc::RequestVoteRPC(
        current_term, server_rank, get_last_log_index(), get_last_log_term());

    mpi::MPI_Broadcast(request_vote.serialize(), 0, MPI_COMM_WORLD, server_rank,
            nb_servers);
}

void Server::leader_heartbeat()
{
    std::cout << "[HEARTBEAT] from leader " << server_rank
              << "------------------------------" << std::endl;

    for (int follower_rank = 1; follower_rank <= nb_servers; follower_rank++)
    {
        if (follower_rank == server_rank)
            continue;

        int prev_log_index = get_prev_log_index(follower_rank);
        int prev_log_term = get_prev_log_term(follower_rank);
        auto entries = std::vector<rpc::LogEntry>();

        auto rpc = std::make_shared<rpc::AppendEntriesRPC>(current_term,
                                                            server_rank,
                                                            prev_log_index,
                                                            prev_log_term,
                                                            entries,
                                                            commit_index);
        mpi::MPI_Serialize_and_send(rpc, follower_rank, 0, MPI_COMM_WORLD);
    }
}

/* ---------------------- Status conversion functions ---------------------- */

void Server::convert_to_follower()
{
    current_status = ServerStatus::FOLLOWER;
    voted_for = 0;

    begin = chrono::get_time_milliseconds();
    reset_election_timeout();
}

void Server::convert_to_leader()
{
    current_status = ServerStatus::LEADER;
    voted_for = 0;

    // For each server, index of the next log entry to send to that server
    // (initialized to leader last log index + 1)
    next_index = std::vector<int>(nb_servers + 1, log.size());

    // For each server, index of highest log entry known to be replicated on
    // server (initialized to 0, increases monotonically)
    match_index = std::vector<int>(nb_servers + 1, -1);

    // XXX: Debugging information
    std::cout << "[LEADER] term : " << current_term << ", rank : " << server_rank
              << std::endl;

    // Rules for Servers - Leaders:
    //      Upon election: send initial empty AppendEntries RPCs (heartbeat)
    //      to each server; repeat during idle periods to prevent election
    //      timeouts (§5.2)
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

int Server::get_prev_log_index(unsigned int rank)
{
    return next_index[rank] - 1;
}

int Server::get_prev_log_term(unsigned int rank)
{
    int index = get_prev_log_index(rank);

    // XXX: Decide if we want to return 0 as first term or -1
    return index >= 0 ? log[index].get_term() : -1;
}

int Server::get_term_at_prev_log_index(int prev_log_index)
{
    if (prev_log_index < 0)
        return -1;

    return log[prev_log_index].get_term();
}

// XXX: For testing purpose
void Server::set_status(const ServerStatus& server_status)
{
    current_status = server_status;
}

void Server::on_client_request(const client::ClientRequest& request)
{
    // XXX: what if not leader are yet elected
    if (get_status() != ServerStatus::LEADER)
    {
        // send back message
        auto response = client::ClientResponse::not_a_leader_response(voted_for);
        std::string message = response.serialize();
        MPI_Send(message.data(), message.size(), MPI_CHAR, request.get_client_index(),
                0, MPI_COMM_WORLD);
    }
    else
    {
        // handle request, send appendentries to follower etc ...
        auto log_entry = rpc::LogEntry(current_term, request.get_command(),
                request.get_client_index() ,request.get_serial_number());
        log.push_back(log_entry);
    }
}


ServerStatus Server::get_status() const
{
    return current_status;
}

bool Server::is_alive() const
{
    return alive;
}
