#include <iostream>

#include "mpi.h"
#include "client/client.hh"
#include "raft/server.hh"
#include "raft/status.hh"
#include "repl/repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

int main(int argc, char* argv[])
{
    int nb_servers = std::stoi(argv[1]);
    int nb_clients = std::stoi(argv[2]);
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        repl::REPL repl;
        repl.run();
    }
    else if (rank >= 1 && rank <= nb_servers)
    {
        Server server(rank, nb_servers);
        std::cout << rank << " : I am a server\n";
        server.run();
    }
    else
    {
        Client client(nb_servers, rank);
        std::cout << rank << " : I am a client\n";
        client.run();
    }

    if (Client::are_client_finished(nb_clients))
        MPI_Abort(MPI_COMM_WORLD, 0);

    MPI_Finalize();
    return 2;

}
