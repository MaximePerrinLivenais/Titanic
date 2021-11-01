#pragma once

#include "client/client-request.hh"

class Client
{
    public:
        Client(const int server_last_index, unsigned int client_index);

        client::ClientRequest create_request(const std::string& command);

        void run();

    private:

        void load_clients_command();
        void process_message(message::shared_msg query);
        void process_repl_message(message::shared_msg query);
        void process_client_message(message::shared_msg query);
        void check_time_since_last_request();

        /* Send request */
        void send_request(const client::ClientRequest& request,
            unsigned int server_index);
        void send_next_request();
        void send_again();


        /* -------- Attributes -------- */
        unsigned int serial_number;
        const unsigned int client_index;
        const int server_last_index;

        unsigned long time_since_last_request;

        bool started = false;

        static const unsigned resend_timeout = 800;

        // Use with std::vector<std::string> commands;
        unsigned next_request = 0;
        unsigned next_response = 0;

        int last_known_leader;

        std::vector<client::ClientRequest> commands;
};
