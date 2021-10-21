#pragma once

#include <queue>
#include <string>
#include <vector>

#include "rpc/append-entries-response.hh"
#include "rpc/append-entries.hh"
#include "rpc/log-entry.hh"
#include "rpc/rpc.hh"
#include "status.hh"

class Server
{
public:
    Server() = default;

    void run();
    ServerStatus get_status();

    void count_vote(const bool vote_granted);

    unsigned int get_term();
    void save_log() const;

    void set_status(ServerStatus status);
    void on_append_entries_rpc(const rpc::AppendEntriesRPC &rpc);
    void on_append_entries_response(const rpc::AppendEntriesResponse &rpc);
    void convert_to_follower();

    bool check_majority();
    void convert_to_leader();

    static unsigned int get_rank();
    void broadcast_rpc();

private:
    int get_last_log_index();
    void check_leader_rules();

    void handle_election_timeout();
    void set_election_timeout();
    void apply_queries(std::vector<rpc::shared_rpc> &queries);

    // Server rules
    void update_commit_index();

    ServerStatus current_status;

    // Election
    unsigned int election_timeout;
    unsigned int vote_count;
    unsigned long begin;

    // Persistent state
    unsigned int current_term;
    unsigned int voted_for;
    std::vector<rpc::LogEntry> log;

    // Volatile state
    unsigned int commit_index;
    unsigned int last_applied;

    // Leader
    std::vector<unsigned int> next_index;
    std::vector<unsigned int> match_index;
};
