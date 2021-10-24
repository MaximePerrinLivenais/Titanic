#pragma once

#include <optional>
#include <string>

#include "mpi.h"
#include "rpc/rpc.hh"

namespace mpi
{
    int MPI_Timeoutrecv(void* buf, int count, MPI_Datatype datatype, int source,
                            int tag, MPI_Comm comm, int timeout);

    std::optional<std::string> MPI_Listen(const MPI_Comm comm);

    int MPI_Serialize_and_send(const rpc::shared_rpc rpc, const int dest,
                                const int tag, const MPI_Comm comm);

    void MPI_Broadcast(const std::string& message, const int tag,
                        const MPI_Comm comm, int rank, int nb_servers);

    int MPI_Get_group_comm_size(const MPI_Comm comm);
    int MPI_Get_group_comm_rank(const MPI_Comm comm);
}
