#pragma once

#include "client-response.hh"
#include "client/client-request.hh"
#include "process/process.hh"

class Client : public Process
{
public:
    Client(const int server_last_index, unsigned int client_index);

    client::ClientRequest create_request(const std::string& command);
    void send_request(const client::ClientRequest& request,
                      unsigned int server_index) const;

    // Message callbck
    void on_repl_start();
    void on_client_response(client::ClientResponse& client_rsp);

    void run();

private:
    void load_clients_command();

    // XXX: map between serial_number and ClientRequest to tracks sent request
    // and remove them when answered
    unsigned int serial_number;
    const unsigned int client_index;
    const int server_last_index;

    // TODO: pass to false + use REPL START
    bool started = false;

    // Use with std::vector<std::string> commands;
    unsigned last_send_request = 0;
    unsigned last_recv_request = 0;

    bool recv_all_response = false;

    std::vector<client::ClientRequest> commands;
};
