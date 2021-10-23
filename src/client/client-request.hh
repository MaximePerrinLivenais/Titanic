#pragma once

#include <string>

namespace client
{
    class ClientRequest
    {
    public:
        ClientRequest(const std::string& command,
                const std::string& serial_number, unsigned int client_index);


        std::string get_command() const;
        std::string get_serial_number() const;
        unsigned int get_client_index() const;

    private:
        const std::string command;
        const std::string serial_number;
        const unsigned int client_index;
    };

}
