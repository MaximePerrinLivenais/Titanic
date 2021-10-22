#pragma once

#include <vector>
#include <queue>
#include <string>

#include "rpc/append-entries.hh"
#include "rpc/append-entries-response.hh"
#include "status.hh"

class Server
{
public:
    Server() = default;

    ServerStatus get_status();

    void count_vote(const bool vote_granted);

    unsigned int get_term();
    void save_log() const;

    void set_status(ServerStatus status);
    void on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc);
    void on_append_entries_response(const rpc::AppendEntriesResponse& rpc);

private:
    int get_last_log_index();
    void check_leader_rules();

    void handle_election_timeout();
    void set_election_timeout();
    void apply_queries(std::vector<rpc::RemoteProcedureCall> &queries);

    void convert_to_follower();
    void convert_to_leader();
    bool check_majority();

    // Server rules
    void update_commit_index();

    ServerStatus current_status;
    unsigned int election_timeout;
    unsigned int vote_count;

    // Persistent state
    unsigned int current_term;
    unsigned int voted_for;
    std::vector<int> log;

    // Volatile state
    unsigned int commit_index;
    unsigned int last_applied;

    // Leader
    std::vector<unsigned int> next_index;
    std::vector<unsigned int> match_index;
};
