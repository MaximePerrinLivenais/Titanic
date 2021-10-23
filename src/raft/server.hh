#pragma once

#include <queue>
#include <string>
#include <vector>

#include "rpc/append-entries-response.hh"
#include "rpc/append-entries.hh"
#include "rpc/log-entry.hh"
#include "rpc/request-vote-response.hh"
#include "rpc/request-vote.hh"
#include "status.hh"

class Server
{
public:
    Server();

    // XXX: For testing purpose
    void set_status(const ServerStatus& server_status);

    void run();
    void save_log() const;

    // Server reactions functions according to RPC type
    void on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc);
    void on_append_entries_response(const rpc::AppendEntriesResponse& rpc);
    void on_request_vote_rpc(const rpc::RequestVoteRPC& rpc);
    void on_request_vote_response(const rpc::RequestVoteResponse& rpc);

private:
    /* ----------- Methods ----------- */

    // Server rules
    void apply_rules();
    void apply_follower_and_candidate_rules();
    void apply_leader_rules();

    // Useful auxiliary functions
    void apply_query(rpc::shared_rpc query);
    void update_commit_index();
    void handle_election_timeout();
    void reset_timer();
    bool check_majority();

    // Messages
    void broadcast_request_vote();
    void leader_heartbeat();

    // Status conversion functions
    void convert_to_follower();
    void convert_to_leader();

    // Useful getters
    int get_last_log_index();
    int get_last_log_term();
    int get_prev_log_index();
    int get_prev_log_term();

    /* ----------- Attributes ----------- */

    static const unsigned int heartbeat_time = 30;

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
    int commit_index;
    int last_applied;

    // Leader
    std::vector<unsigned int> next_index;
    std::vector<unsigned int> match_index;
};
