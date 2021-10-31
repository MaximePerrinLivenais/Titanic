#pragma once

#include <vector>

#include "client-response.hh"
#include "process/process.hh"

// XXX: A rm quand plus besoin de send_request
#include "client/client-request.hh"

namespace client
{
    class Client : public process::Process
    {
    public:
        Client(const int server_last_index, unsigned int client_index);

        void run();

        // Message callbck
        void on_repl_start();
        void on_client_response(const ClientResponse& client_rsp);

        shared_client_request create_request(const std::string& command);

    private:
        void load_clients_command();

        // XXX: map between serial_number and ClientRequest to tracks sent
        // request and remove them when answered
        unsigned int serial_number;
        const int server_last_index;

        // TODO: pass to false + use REPL START
        bool started = false;

        // Use with std::vector<std::string> commands;
        unsigned last_send_request = 0;
        unsigned last_recv_request = 0;

        bool recv_all_response = false;

        std::vector<shared_client_request> commands;
    };
} // namespace client
