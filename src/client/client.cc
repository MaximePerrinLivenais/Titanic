#include "client.hh"

Client::Client(const int server_last_index, unsigned int client_index) :
    serial_number(0), client_index(client_index), server_last_index(server_last_index)
{
}

client::ClientRequest Client::create_request(const std::string& command)
{
    serial_number++;
    return client::ClientRequest(command, serial_number, client_index);
}
