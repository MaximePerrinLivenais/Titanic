#include <iostream>

#include "client/client.hh"
#include "mpi.h"
#include "raft/server.hh"
#include "raft/status.hh"
#include "repl/repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

int main(int argc, char* argv[])
{
    int nb_servers = std::stoi(argv[1]);
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
        std::cout << "Hi, I am rank: " << rank << std::endl;
        Server server(rank, nb_servers);
        server.run();
    }
    else
    {
        Client client(nb_servers, rank);
        std::cout << rank << " : I am a client\n";
        client.run();
    }

    MPI_Finalize();
}
