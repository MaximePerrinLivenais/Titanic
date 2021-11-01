#include "mpi-wrapper.hh"

#include <iostream>
#include <vector>

namespace mpi
{
    int MPI_Timeoutrecv(void *buf, int count, MPI_Datatype datatype, int source,
                        int tag, MPI_Comm comm, int timeout)
    {
        MPI_Request request;
        MPI_Irecv(buf, count, datatype, source, tag, comm, &request);

        int request_is_completed = 0;
        double time = MPI_Wtime();
        MPI_Status status;

        while (timeout > MPI_Wtime() - time && !request_is_completed)
            MPI_Test(&request, &request_is_completed, &status);

        if (request_is_completed)
            return MPI_SUCCESS;

        MPI_Cancel(&request);
        return status.MPI_ERROR;
    }

    std::optional<std::string> MPI_Listen(const MPI_Comm comm)
    {
        int flag;
        MPI_Status status;
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, comm, &flag, &status);

        auto source = status.MPI_SOURCE;
        auto tag = status.MPI_TAG;

        if (!flag)
            return std::nullopt;

        int count = 0;
        MPI_Get_count(&status, MPI_CHAR, &count);

        auto buffer = std::vector<char>(count);
        MPI_Recv(buffer.data(), count, MPI_CHAR, source, tag, comm, &status);

        return std::make_optional<std::string>(buffer.data(), count);
    }

    int MPI_Serialize_and_send(const rpc::shared_rpc rpc, const int dest,
                                const int tag, const MPI_Comm comm)
    {
        auto message = rpc->serialize();

        return MPI_Send(message.data(), message.length(), MPI_CHAR, dest, tag, comm);
    }

    void MPI_Broadcast(const std::string &message, const int tag,
                       const MPI_Comm comm, int rank, int nb_servers)
    {
        for (int i = 1; i <= nb_servers; i++)
        {
            if (i != rank)
            {
                MPI_Send(message.c_str(), message.length(), MPI_CHAR, i, tag,
                         comm);
            }
        }
    }

    int MPI_Get_group_comm_size(const MPI_Comm comm)
    {
        int size;
        MPI_Comm_size(comm, &size);

        return size;
    }

    int MPI_Get_group_comm_rank(const MPI_Comm comm)
    {
        int rank;
        MPI_Comm_rank(comm, &rank);

        return rank;
    }
} // namespace mpi

