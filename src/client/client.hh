#pragma once

#include <vector>

#include "client-response.hh"
#include "client-finish.hh"
#include "client-request.hh"
#include "process/process.hh"

namespace client
{
    class Client : public process::Process
    {
    public:
        Client(const int server_last_index, unsigned int client_index);

        void run();

        static bool are_client_finished(int nb_clients);

        // Message callbck
        void on_repl_start();
        void on_repl_send();
        void on_client_request(const ClientFinish& client_fin);
        void on_client_response(const ClientResponse& client_rsp);

        shared_client_request create_request(const std::string& command);

    private:
        void load_clients_command();



        void check_time_since_last_request();
        void notify_finish_to_all_clients();

        /* Send request */
        void send_request(const shared_client_request& request,
                unsigned int server_index);
        void send_next_request();
        void send_again();


        /* -------- Attributes -------- */
        unsigned int serial_number;
        const int server_last_index;

        unsigned long time_since_last_request;

        bool started = false;

        static const unsigned resend_timeout = 1500;

        // Use with std::vector<std::string> commands;
        unsigned next_request = 0;
        unsigned next_response = 0;

        int last_known_leader;

        std::vector<shared_client_request> commands;

        static int client_finished;
    };
} // namespace client
