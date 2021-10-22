#include <iostream>

#include "mpi.h"
#include "client/client.hh"
#include "raft/server.hh"
#include "raft/status.hh"
#include "utils/openmpi/mpi-wrapper.hh"

int main(int argc, char *argv[])
{
    int nb_servers = std::stoi(argv[1]);
    int rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        // REPL
    }
    else if (rank >= 1 && rank <= nb_servers)
    {
        Server server;
        std::cout << rank << " : I am a server\n";
    }
    else
    {
        Client client(nb_servers);
        std::cout << rank << " : I am a client\n";
    }


    MPI_Finalize();
}
