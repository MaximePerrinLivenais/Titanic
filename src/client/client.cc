#include "client.hh"

#include <iostream>
#include <sstream>
#include <unistd.h>

#include "utils/openmpi/mpi-wrapper.hh"

namespace client
{
    Client::Client(const int server_last_index, unsigned int client_rank)
        : Process(client_rank)
        , serial_number(0)
        , server_last_index(server_last_index)
    {
        load_clients_command();
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

                mpi::MPI_Serialize_and_send(request, server_index, 0,
                                            MPI_COMM_WORLD);

                last_send_request++;
            }

            auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
            if (query_str_opt.has_value())
            {
                auto query =
                    message::Message::deserialize(query_str_opt.value());
                query->apply(*this);
            }
        }
        std::cout << "Client " << rank << " finished it's journey\n";

        while (1)
        {}
    }

    void Client::on_repl_start()
    {
        started = true;
    }

    void Client::on_client_response(const ClientResponse& client_rsp)
    {
        if (client_rsp.is_success())
        {
            last_recv_request++;
            std::cout << "Recv response : " << last_recv_request << "\n";
        }
        else
        {
            // Maybe use the same serial_number
            // -1 here because we increment last_send_request on first send
            auto request = commands[last_send_request - 1];
            mpi::MPI_Serialize_and_send(request, client_rsp.get_leader_id(), 0,
                                        MPI_COMM_WORLD);
        }
    }

    shared_client_request Client::create_request(const std::string& command)
    {
        serial_number++;
        return std::make_shared<ClientRequest>(command, serial_number, rank);
    }

    void Client::load_clients_command()
    {
        std::string filename =
            "client_commands/commands_" + std::to_string(rank) + ".txt";

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
} // namespace client
