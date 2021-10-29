#pragma once

#include "client/client-request.hh"

class Client
{
public:
    Client(const int server_last_index, unsigned int client_index);

    client::ClientRequest create_request(const std::string& command);
    void send_request(const client::ClientRequest& request,
                      unsigned int server_index) const;

    void run();

private:
    void load_clients_command();
    void handle_message(message::shared_msg query);
    // XXX: map between serial_number and ClientRequest to tracks sent request
    // and remove them when answered
    unsigned int serial_number;
    const unsigned int client_index;
    const int server_last_index;

    // Message callback
    void on_repl_start();

    // TODO: pass to false + use REPL START
    bool started = false;

    // Use with std::vector<std::string> commands;
    unsigned last_send_request = 0;
    unsigned last_recv_request = 0;

    bool recv_all_response = false;

    std::vector<client::ClientRequest> commands;
};
