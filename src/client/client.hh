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

        /* Send request */
        void send_request(const client::ClientRequest& request,
            unsigned int server_index) const;
        void send_next_request();
        void send_again();


        /* -------- Attributes -------- */
        unsigned int serial_number;
        const unsigned int client_index;
        const int server_last_index;


        // TODO: pass to false + use REPL START
        bool started = false;


        // Use with std::vector<std::string> commands;
        unsigned next_request = 0;
        unsigned next_response = 0;

        int last_known_leader;


        bool recv_all_response = false;


        std::vector<client::ClientRequest> commands;
};
