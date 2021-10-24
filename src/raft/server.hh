#pragma once

#include <queue>
#include <string>
#include <vector>

#include "message/message.hh"
#include "repl/repl.hh"
#include "rpc/append-entries-response.hh"
#include "rpc/append-entries.hh"
#include "rpc/log-entry.hh"
#include "rpc/request-vote-response.hh"
#include "rpc/request-vote.hh"
#include "rpc/rpc.hh"
#include "client/client-request.hh"
#include "status.hh"

// namespace rpc
//{
//    class RemoteProcedureCall;
//    using shared_rpc = std::shared_ptr<RemoteProcedureCall>;
//
//    class AppendEntriesRPC;
//    class AppendEntriesResponse;
//    class LogEntry;
//    class RequestVoteRPC;
//    class RequestVoteResponse;
//} // namespace rpc

class Server
{
public:
    Server(int nb_servers, int server_rank);

    // XXX: For testing purpose
    void set_status(const ServerStatus& server_status);
    ServerStatus get_status() const;

    void run();
    void save_log() const;

    // Server reactions functions according to RPC type
    void on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc);
    void on_append_entries_response(const rpc::AppendEntriesResponse& rpc);
    void on_request_vote_rpc(const rpc::RequestVoteRPC& rpc);
    void on_request_vote_response(const rpc::RequestVoteResponse& rpc);
    void on_client_request(const client::ClientRequest& request);

    bool is_alive() const;
    void crash();
    void change_speed(repl::ServerSpeed speed);
    void update_term(unsigned int term);

private:

    /* ----------- Methods ----------- */

    int get_prev_log_index();
    int get_prev_log_term();

    // Server rules
    void apply_rules();
    void apply_follower_and_candidate_rules();
    void apply_leader_rules();

    // Useful auxiliary functions
    void set_current_term(const int term);
    void apply_query(message::shared_msg query);
    void update_commit_index();
    void convert_to_candidate();
    void reset_election_timeout();
    bool check_majority();
    bool candidate_log_is_up_to_date(int last_log_index, int last_log_term);

    // Messages
    void broadcast_request_vote();
    void leader_heartbeat();

    // Status conversion functions
    void convert_to_follower();
    void convert_to_leader();

    // Useful getters
    int get_last_log_index();
    int get_last_log_term();
    int get_prev_log_index(unsigned int rank);
    int get_prev_log_term(unsigned int rank);
    int get_term_at_prev_log_index(int prev_log_index);

    // Repl modifiers
    // XXX: maybe add crash to ServerStatus or use ElectionStatus + ServerStatus
    bool alive = true;
    int latency = 0;


    /* ----------- Attributes ----------- */

    // To avoid calling MPI_Comm_rank every time
    int server_rank;

    // Number of servers, servers rank go from 1 to nb_servers
    int nb_servers;

    static const unsigned int heartbeat_time = 300;

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
    std::vector<int> next_index;
    std::vector<int> match_index;
};
