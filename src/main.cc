#include <iostream>
#include <chrono>
#include <thread>

#include "client/client.hh"
#include "raft/server.hh"
#include "repl/repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

int main(int argc, char* argv[])
{
    int nb_servers = std::stoi(argv[1]);
    int nb_clients = std::stoi(argv[2]);

    MPI_Init(&argc, &argv);
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);

    if (rank == 0)
    {
        auto repl = repl::REPL();
        repl.run();
    }
    else if (rank >= 1 && rank <= nb_servers)
    {
        std::cout << rank << " : I am a server\n";
        auto server = raft::Server(rank, nb_servers);
        server.run();
    }
    else
    {
        std::cout << rank << " : I am a client\n";
        auto client = client::Client(nb_servers, rank);
        client.run();
    }

    /* If all clients finished we shutdown the system */
    if (client::Client::are_client_finished(nb_clients))
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "System ends\n";
        MPI_Abort(MPI_COMM_WORLD, 0);
    }

    MPI_Finalize();

    return 0;
}
