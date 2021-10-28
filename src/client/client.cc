#include "client.hh"

#include <iostream>
#include <sstream>
#include <unistd.h>

#include "client-response.hh"
#include "mpi.h"
#include "repl/repl-message.hh"
#include "utils/openmpi/mpi-wrapper.hh"

Client::Client(const int server_last_index, unsigned int client_index)
    : serial_number(0)
    , client_index(client_index)
    , server_last_index(server_last_index)
{
    load_clients_command();
}

void Client::load_clients_command()
{
    std::string filename =
        "client_commands/commands_" + std::to_string(client_index) + ".txt";

    MPI_File file;
    MPI_Status status;
    // TODO: Check if it is open

    MPI_File_open(MPI_COMM_SELF, filename.data(), MPI_MODE_RDONLY,
                  MPI_INFO_NULL, &file);

    MPI_Offset size;
    MPI_File_get_size(file, &size);

    std::vector<char> buf = std::vector<char>(size);
    MPI_File_read(file, buf.data(), size, MPI_CHAR, &status);

    std::string commands_data(buf.begin(), buf.end());
    std::stringstream stream(commands_data);

    std::string line;
    while (std::getline(stream, line))
        commands.push_back(create_request(line));
}

client::ClientRequest Client::create_request(const std::string& command)
{
    serial_number++;
    return client::ClientRequest(command, serial_number, client_index);
}

void Client::send_request(const client::ClientRequest& request,
                          unsigned int server_index) const
{
    std::string message = request.serialize();
    MPI_Send(message.data(), message.size(), MPI_CHAR, server_index, 0,
             MPI_COMM_WORLD);
}

void Client::run()
{
    while (last_recv_request < commands.size())
    {
        // sleep(1);
        if (last_send_request == last_recv_request && started)
        {
            // send next request
            std::cout << "Send request " << last_send_request << "\n";
            auto request = commands[last_send_request];
            int server_index = 1; // TODO: choose randomly
            send_request(request, server_index);
            last_send_request++;
        }

        auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        if (query_str_opt.has_value())
        {
            auto query = message::Message::deserialize(query_str_opt.value());
            handle_message(query);
        }
    }
    std::cout << "Client " << client_index << " finished it's journey\n";

    while (1)
    {}
}

void Client::handle_message(message::shared_msg query)
{
    if (query->get_msg_type() == REPL_MESSAGE)
    {
        auto repl_msg = std::dynamic_pointer_cast<repl::ReplMsg>(query);
        if (repl_msg->get_repl_msg_type() == repl::START)
            started = true;
    }
    else if (query->get_msg_type() == CLIENT_MESSAGE)
    {
        auto client_msg = std::dynamic_pointer_cast<client::ClientMsg>(query);
        if (client_msg->get_client_msg_type() == client::CLIENT_RESPONSE)
        {
            auto client_rsp =
                std::dynamic_pointer_cast<client::ClientResponse>(client_msg);
            if (client_rsp->is_success())
            {
                last_recv_request++;
                std::cout << "Recv response : " << last_recv_request << "\n";
            }
            else
            {
                // Maybe use the same serial_number
                // -1 here because we increment last_send_request on first send
                auto request = commands[last_send_request - 1];
                send_request(request, client_rsp->get_leader_id());
            }
        }
    }
}
