#include "client.hh"
#include "mpi.h"
#include "utils/openmpi/mpi-wrapper.hh"
#include "repl/repl-message.hh"
#include "client-response.hh"
#include <sstream>

#include <unistd.h>

Client::Client(const int server_last_index, unsigned int client_index) :
    serial_number(0), client_index(client_index), server_last_index(server_last_index)
{
    load_clients_command();

    // TODO: use mt19937 instead
    srand(time(0) + client_index);
    last_known_leader = (rand() % server_last_index) + 1;
}


void Client::run()
{
    while (next_request < commands.size())
    {
        //sleep(1);
        if (started && next_request == next_response)
            send_next_request();


        auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        if (query_str_opt.has_value())
        {
            auto query = message::Message::deserialize(query_str_opt.value());
            process_message(query);
        }
    }
    std::cout << "Client " << client_index << " finished it's journey\n";

    while(1){}
}

/* ---------------------- Request creation and load ------------------------- */
void Client::load_clients_command()
{
    std::string filename = "client_commands/commands_"
        + std::to_string(client_index) + ".txt";

    MPI_File file;
    MPI_Status status;
    // TODO: Check if it is open

    MPI_File_open(MPI_COMM_SELF, filename.data(),
            MPI_MODE_RDONLY, MPI_INFO_NULL, &file);

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




/* --------------------------- Send request --------------------------------- */
void Client::send_request(const client::ClientRequest& request,
        unsigned int server_index) const
{
    std::cout << "Client n" << client_index << " send message to server n" << server_index << "\n";
    std::string message = request.serialize();
    MPI_Send(message.data(), message.size(), MPI_CHAR, server_index, 0, MPI_COMM_WORLD);
}

void Client::send_next_request()
{
    std::cout << "send next\n";
    if (next_request > next_response + 1)
    {
        std::cout << "Waiting for previous request to get a response\n";
        return;
    }

    std::cout << "afetr if\n";
    auto request = commands[next_request];
    send_request(request, last_known_leader);
    next_request++;
}

void Client::send_again()
{
    auto request = commands[next_response];
    send_request(request, last_known_leader);
}


/* ---------------------- Process received message -------------------------- */
void Client::process_message(message::shared_msg message)
{
    if (message->get_msg_type() == REPL_MESSAGE)
        process_repl_message(message);
    else if (message->get_msg_type() == CLIENT_MESSAGE)
        process_client_message(message);
}

void Client::process_repl_message(message::shared_msg message)
{
    auto repl_msg = std::dynamic_pointer_cast<repl::ReplMsg>(message);

    std::cout << "Repl blabla\n";
    if (repl_msg->get_repl_msg_type() == repl::START)
        started = true;
    else if (repl_msg->get_repl_msg_type() == repl::SEND)
        send_next_request();
}

void Client::process_client_message(message::shared_msg message)
{
    auto client_msg = std::dynamic_pointer_cast<client::ClientMsg>(message);

    if (client_msg->get_client_msg_type() != client::CLIENT_RESPONSE)
        return;


    auto client_rsp = std::dynamic_pointer_cast<client::ClientResponse>(client_msg);
    if (client_rsp->is_success())
    {
        next_response++;
        std::cout << "Recv response : " << next_request << "\n";
    }
    else
    {
        last_known_leader = client_rsp->get_leader_id();
        send_again();
    }
}
