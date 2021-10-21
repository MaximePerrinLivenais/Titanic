#pragma once

#include <queue>
#include <string>
#include <vector>

#include "rpc/rpc.hh"
#include "rpc/append-entries.hh"
#include "status.hh"

class Server
{
public:
    Server();

    ServerStatus get_status();

    void count_vote(const bool vote_granted);

    unsigned int get_term();
    void save_log() const;

    void set_status(ServerStatus status);
    void on_append_entries_rpc(const rpc::AppendEntriesRPC& rpc);

private:
    void handle_election_timeout();
    void set_election_timeout();
    void apply_queries(std::vector<rpc::RemoteProcedureCall> &queries);

    void convert_to_follower();
    void convert_to_leader();
    bool check_majority();

    ServerStatus current_status;
    unsigned int election_timeout;
    unsigned int vote_count;

    // Persistent state
    unsigned int current_term;
    unsigned int voted_for;
    std::vector<std::string> log;

    // Volatile state
    unsigned int commit_index;
    unsigned int last_applied;

    // Leader
    std::vector<unsigned int> next_index;
    std::vector<unsigned int> match_index;
};
