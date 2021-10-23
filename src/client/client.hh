#pragma once

#include "client/client-request.hh"

class Client
{
    public:
        Client(const int server_last_index, unsigned int client_index);

        client::ClientRequest create_request(const std::string& command);
        //void send_request();

    private:
        unsigned int serial_number;
        const unsigned int client_index;
        const int server_last_index;
};
