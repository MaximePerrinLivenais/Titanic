#pragma once

#include "mpi.h"

namespace mpi
{
int MPI_Timeoutrecv(void* buf,
                        int count,
                        MPI_Datatype datatype,
                        int source,
                        int tag,
                        MPI_Comm comm,
                        int timeout);
}
