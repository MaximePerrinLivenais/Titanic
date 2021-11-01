#include <iostream>

#include "client/client.hh"
#include "raft/server.hh"
#include "repl/repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

int main(int argc, char* argv[])
{
    int nb_servers = std::stoi(argv[1]);

    MPI_Init(&argc, &argv);
    int rank = mpi::MPI_Get_group_comm_rank(MPI_COMM_WORLD);

    if (rank == 0)
    {
        auto repl = repl::REPL();
        repl.run();
    }
    else if (rank >= 1 && rank <= nb_servers)
    {
        std::cout << "Hi, I am rank: " << rank << std::endl;
        auto server = raft::Server(rank, nb_servers);
        server.run();
    }
    else
    {
        auto client = client::Client(nb_servers, rank);
        std::cout << rank << " : I am a client\n";
        client.run();
    }

    MPI_Finalize();
}
