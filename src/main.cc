#include <iostream>

#include "mpi.h"
#include "raft/server.hh"
#include "raft/status.hh"
#include "repl/repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    char process_name[MPI_MAX_PROCESSOR_NAME];
    int process_name_len;
    MPI_Get_processor_name(process_name, &process_name_len);

    std::cout << "Hello world from processor " << process_name << " rank "
              << rank << " out of " << size << " processors\n";

    if (rank == 0)
    {
        repl::REPL repl;
        repl.run();
    }
    else
    {
        Server server;
        server.run();
    }

    // ---
    /*server.set_status(ServerStatus::FOLLOWER);
    if (rank == 0)
    {
        server.set_status(ServerStatus::LEADER);

        std::vector<rpc::LogEntry> entries = {};
        rpc::AppendEntriesRPC rpc(0, 0, 0, 0, entries, 0);

        // send RPC
        std::string message = rpc.serialize();
        mpi::MPI_Broadcast(message, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    else
    {
        MPI_Barrier(MPI_COMM_WORLD);
        auto message = mpi::MPI_Listen(MPI_COMM_WORLD);
        if (message.has_value())
        {
            rpc::shared_rpc rpc =
                rpc::RemoteProcedureCall::deserialize(message.value());
            rpc->apply(server);
        }
    }

    std::cout << rank << ": Save log\n";
    server.save_log();*/

    MPI_Finalize();
}
