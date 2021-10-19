#include "utils/openmpi/mpi-wrapper.hh"

using namespace mpi;

int MPI_Timeoutrecv(void* buf,
                        int count,
                        MPI_Datatype datatype,
                        int source,
                        int tag,
                        MPI_Comm comm,
                        int timeout)
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
