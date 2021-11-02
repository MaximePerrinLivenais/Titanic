#include "client.hh"
#include "mpi.h"
#include "utils/openmpi/mpi-wrapper.hh"
#include "repl/repl-message.hh"
#include "client-response.hh"
#include "client-request.hh"
#include "client-finish.hh"
#include "utils/chrono/chrono.hh"
#include <sstream>
#include <thread>
#include <chrono>


#include <iostream>
#include <sstream>
#include <unistd.h>


namespace client
{
    int Client::client_finished = 0;

    Client::Client(const int server_last_index, unsigned int client_index) :
        process::Process(client_index),
        serial_number(0), server_last_index(server_last_index)
    {
        load_clients_command();

        // TODO: use mt19937 instead
        srand(time(0) + rank);
        last_known_leader = (rand() % server_last_index) + 1;
    }

    void Client::run()
    {

        while (next_request < commands.size())
        {
            if (started && next_request == next_response)
            {
                //std::this_thread::sleep_for(std::chrono::milliseconds(200));
                send_next_request();
            }



            auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
            if (query_str_opt.has_value())
            {
                auto query =
                    message::Message::deserialize(query_str_opt.value());
                query->apply(*this);
            }

            check_time_since_last_request();
        }
        std::cout << "Client " << rank << " finished it's journey\n";

        notify_finish_to_all_clients();
        std::cout << Client::client_finished << " finished\n";
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

    bool Client::are_client_finished(int nb_clients)
    {
        std::cout << "Client_finished = " << client_finished << " | " << nb_clients << "\n";
        return client_finished >= nb_clients;
    }

    void Client::notify_finish_to_all_clients()
    {
        Client::client_finished++;
        unsigned size = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);

        auto request = ClientFinish();
        auto message = request.serialize();
        MPI_Request req;

        for (unsigned client_rank = server_last_index + 1; client_rank < size; client_rank++)
        {
            if (client_rank != rank)
                MPI_Isend(message.data(), message.size(), MPI_CHAR, client_rank,
                        0, MPI_COMM_WORLD, &req);
        }
    }

    /* ---------------------- Request creation and load ------------------------- */

    void Client::load_clients_command()
    {
        std::string filename =
            "client_commands/commands_" + std::to_string(rank) + ".txt";

        MPI_File file;
        MPI_Status status;
        // TODO: Check if it is open

        auto rc = MPI_File_open(MPI_COMM_SELF, filename.data(), MPI_MODE_RDONLY,
                MPI_INFO_NULL, &file);

        if (rc)
            std::cout << "ERROR WHILE OPENING : " << filename
                << "==================================\n"
                << std::flush;

        MPI_Offset size;
        MPI_File_get_size(file, &size);

        std::vector<char> buf = std::vector<char>(size);
        MPI_File_read(file, buf.data(), size, MPI_CHAR, &status);

        std::string commands_data(buf.begin(), buf.end());
        std::stringstream stream(commands_data);

        std::string line;
        while (std::getline(stream, line))
            commands.push_back(create_request(line));

        MPI_File_close(&file);
    }

    shared_client_request Client::create_request(const std::string& command)
    {
        serial_number++;
        return std::make_shared<ClientRequest>(command, serial_number, rank);
    }

    /* --------------------------- Send request --------------------------------- */
    void Client::send_request(const shared_client_request& request,
            unsigned int server_index)
    {
        std::cout << "Client n" << rank << " send message to server n" << server_index << "\n";
        std::string message = request->serialize();
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
    void Client::on_repl_start()
    {
        started = true;
    }

    void Client::on_repl_send()
    {
        if (next_request >= commands.size())
            return;
        send_next_request();
    }

    void Client::on_client_request([[maybe_unused]] const ClientFinish& client_fin)
    {
        std::cout << "Received finished request on client" << rank << "\n";
        Client::client_finished++;
    }

    void Client::on_client_response(const ClientResponse& client_rsp)
    {
        if (client_rsp.is_success())
        {
            std::cout << "Recv response : " << next_response << "\n";
            next_response++;
        }
        else
        {
            last_known_leader = client_rsp.get_leader_id();
            send_again();
        }
    }
} // namespace client
