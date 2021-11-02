#include "client.hh"
#include "mpi.h"
#include "utils/openmpi/mpi-wrapper.hh"
#include "repl/repl-message.hh"
#include "client-response.hh"
#include "utils/chrono/chrono.hh"
#include <sstream>
#include <thread>
#include <chrono>

#include <unistd.h>

int Client::client_finished = 0;

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
        {
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            send_next_request();
        }


        auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
        if (query_str_opt.has_value())
        {
            auto query = message::Message::deserialize(query_str_opt.value());
            process_message(query);
        }

        check_time_since_last_request();
    }
    std::cout << "Client " << client_index << " finished it's journey\n";

    notify_finish_to_all_clients();
}

void Client::notify_finish_to_all_clients()
{
    Client::client_finished++;
    unsigned size = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);

    auto request = create_request("finished");
    auto message = request.serialize();
    MPI_Request req;

    for (unsigned rank = server_last_index + 1; rank < size; rank++)
    {
        if (rank != client_index)
            MPI_Isend(message.data(), message.size(), MPI_CHAR, rank,
                    0, MPI_COMM_WORLD, &req);
    }
}


void Client::check_time_since_last_request()
{
    if (next_request != next_response + 1)
        return;

    if (chrono::get_time_milliseconds() - time_since_last_request >= resend_timeout)
    {
        time_since_last_request = chrono::get_time_milliseconds();

        last_known_leader = (rand() % server_last_index) + 1;
        send_again();
    }
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

bool Client::are_client_finished(int nb_clients)
{
    std::cout << "Client_finished = " << client_finished << " | " << nb_clients << "\n";
    return client_finished >= nb_clients;
}


/* --------------------------- Send request --------------------------------- */
void Client::send_request(const client::ClientRequest& request,
        unsigned int server_index)
{
    std::cout << "Client n" << client_index << " send message to server n" << server_index << "\n";
    std::string message = request.serialize();
    MPI_Request req;
    MPI_Isend(message.data(), message.size(), MPI_CHAR, server_index, 0, MPI_COMM_WORLD,
            &req);
    time_since_last_request = chrono::get_time_milliseconds();
}

void Client::send_next_request()
{
    if (next_request > next_response + 1)
    {
        std::cout << "Waiting for previous request to get a response\n";
        return;
    }

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

    if (repl_msg->get_repl_msg_type() == repl::START)
        started = true;
    else if (repl_msg->get_repl_msg_type() == repl::SEND)
        send_next_request();
}

void Client::process_client_message(message::shared_msg message)
{
    auto client_msg = std::dynamic_pointer_cast<client::ClientMsg>(message);

    if (client_msg->get_client_msg_type() != client::CLIENT_RESPONSE)
    {
        std::cout << "Received finished request on client" << client_index << "\n";
        Client::client_finished++;
        return;
    }


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
