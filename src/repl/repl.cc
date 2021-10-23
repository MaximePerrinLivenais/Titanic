#include "repl.hh"

#include <iostream>
#include <string>

#include "request-crash-repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    void REPL::run()
    {
        // Start repl
        while (true)
        {
            std::cout << "Enter command:\n";

            std::string command;
            // Use of getline since cin breaks input for each space encountered.
            std::getline(std::cin, command);

            // TODO Handle the given command (Create the corresponding class)
            std::cout << "You entered the following command:\n" << command;

            // FIXME now we sent a crash request everytime
            RequestCrashREPL request;
            auto s = request.serialize();

            std::cout << s << std::endl;

            // Send this request to the first process for now
            MPI_Send(s.c_str(), s.length(), MPI_CHAR, 1, 0, MPI_COMM_WORLD);

            // rpc::RequestCrash req;
            // auto serialized = req.serialize();

            // MPI_Send(serialized.c_str(), serialized.length(), MPI_CHAR, 1, 0,
            //          MPI_COMM_WORLD);

            // // XXX MAYBE block until we have a response from the server
            // auto query_str_opt = mpi::MPI_Listen(MPI_COMM_WORLD);
            // if (query_str_opt.has_value())
            // {
            //     // XXX Here trash broadcasts not meant for the repl
            //     // auto query =
            //     //
            //     rpc::RemoteProcedureCall::deserialize(query_str_opt.value());

            //     // query->apply(*this);
            // }
        }
    }
} // namespace repl
