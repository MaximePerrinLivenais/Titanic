#pragma once

#include <queue>
#include <string>
#include <vector>

#include "rpc/log-entry.hh"
#include "rpc/rpc.hh"
#include "status.hh"

class Server
{
public:
    Server();

    void run();
    ServerStatus get_status();

    void count_vote(const bool vote_granted);

    unsigned int get_term();
    unsigned int get_voted_for();
    void set_voted_for(const unsigned int voted_for);

    void convert_to_follower();

    bool check_majority();
    void convert_to_leader();

    static unsigned int get_rank();
    void broadcast_request_vote();

private:
    void handle_election_timeout();
    void set_election_timeout();
    void apply_queries(std::vector<rpc::shared_rpc> &queries);

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
