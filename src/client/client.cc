#include "client.hh"
#include "mpi.h"
#include "utils/openmpi/mpi-wrapper.hh"
#include "repl/repl-message.hh"
#include "client-response.hh"

Client::Client(const int server_last_index, unsigned int client_index) :
    serial_number(0), client_index(client_index), server_last_index(server_last_index)
{
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
    MPI_Send(message.data(), message.size(), MPI_CHAR, server_index, 0, MPI_COMM_WORLD);
}

void Client::run()
{
    while (last_recv_request == 0)
    {
        if (last_send_request == 0 && started)
        {
            // send next request
            auto request = create_request("command random");
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
    std::cout << "Client " << client_index << " finished is journey\n";
    while(1){}
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
            auto client_rsp = std::dynamic_pointer_cast<client::ClientResponse>(client_msg);
            if (client_rsp->is_success())
                last_recv_request++;
            else
            {
                // Maybe use the same serial_number 
                auto request = create_request("command random bis");
                send_request(request, client_rsp->get_leader_id());
            }
        }

    }
}
