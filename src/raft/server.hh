#pragma once

#include <string>
#include <vector>

#include "client/client-request.hh"
#include "process/process.hh"
#include "repl/request-speed-repl.hh"
#include "rpc/append-entries-response.hh"
#include "rpc/append-entries.hh"
#include "rpc/request-vote-response.hh"
#include "rpc/request-vote.hh"
#include "status.hh"

namespace raft
{
    class Server : public process::Process
    {
    public:
        Server(const unsigned int server_rank, const unsigned int nb_servers);

        // XXX: For testing purpose
        void set_status(const ServerStatus& server_status);
        ServerStatus get_status() const;

        void run();
        void save_log() const;

        // Server reactions functions according to RPC type
        void on_rpc(rpc::RemoteProcedureCall& rpc);
        void on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc);
        void on_append_entries_response(const rpc::AppendEntriesResponse& rpc);
        void on_request_vote_rpc(const rpc::RequestVoteRPC& rpc);
        void on_request_vote_response(const rpc::RequestVoteResponse& rpc);

        void on_client_request(const client::ClientRequest& request);

        void on_repl_crash();
        void on_repl_speed(const repl::RequestSpeedREPL& repl);
        void on_repl_recovery();

        bool is_alive() const;

    private:
        /* ----------- Methods ----------- */

        // Server rules
        void apply_rules();
        void apply_follower_and_candidate_rules();
        void apply_leader_rules();

        // Useful auxiliary functions
        void set_current_term(const int term);
        void apply_query(message::shared_msg query);
        void update_commit_index();
        void reset_election_timeout();
        bool check_majority();
        bool candidate_log_is_up_to_date(int last_log_index, int last_log_term);

        // Messages
        void broadcast_request_vote();
        void leader_heartbeat();

        // Status conversion functions
        void convert_to_candidate();
        void convert_to_follower();
        void convert_to_leader();

        // Useful getters
        int get_last_log_index();
        int get_last_log_term();
        int get_prev_log_index(unsigned int rank);
        int get_prev_log_term(unsigned int rank);
        int get_term_at_prev_log_index(int prev_log_index);

        // Repl modifiers
        // ServerStatus
        bool alive = true;
        int latency = 0;

        /* ----------- Attributes ----------- */

        // Number of servers, servers rank go from 1 to nb_servers
        const unsigned int nb_servers;

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
        std::vector<int> next_index;
        std::vector<int> match_index;
    };
} // namespace raft
