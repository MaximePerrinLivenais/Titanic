#include "mpi.h"
#include "raft/server.hh"
#include "raft/status.hh"

#include <iostream>

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char process_name[MPI_MAX_PROCESSOR_NAME];
    int process_name_len;
    MPI_Get_processor_name(process_name, &process_name_len);

    std::cout << "Hello world from processor " << process_name
        <<  " rank " << rank
        << " out of " << size
        << " processors\n";



    Server server;
    server.set_status(ServerStatus::FOLLOWER);
    if (rank == 0)
    {

        server.set_status(ServerStatus::LEADER);
    }
    else
    {
        std::cout << "Follower\n";
    }



    MPI_Finalize();
}
