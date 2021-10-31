#include "repl.hh"

#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "request-crash-repl.hh"
#include "request-speed-repl.hh"
#include "request-start-repl.hh"
#include "server-speed.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    static std::optional<shared_repl_msg> process_request(std::string& command)
    {
        if (command == "CRASH")
            return std::make_shared<RequestCrashREPL>();
        else if (command == "SPEED")
        {
            // Handle speed message
            std::string speed_str;
            std::cin >> speed_str;
            ServerSpeed speed;

            if (speed_str == "LOW")
                speed = LOW;
            else if (speed_str == "MEDIUM")
                speed = MEDIUM;
            else if (speed_str == "HIGH")
                speed = HIGH;
            else
            {
                std::cout << "Invalid speed option\n";
                return std::nullopt;
            }

            return std::make_shared<RequestSpeedREPL>(speed);
        }
        else if (command == "START")
            return std::make_shared<RequestStartREPL>();

        return std::nullopt;
    }

    static void process_msg_from_input()
    {
        std::string command_name;
        std::cin >> command_name;

        std::optional<shared_repl_msg> repl_msg = process_request(command_name);

        if (repl_msg.has_value())
        {
            unsigned target_process = 0;
            std::cin >> target_process;

            mpi::MPI_Serialize_and_send(repl_msg.value(), target_process, 0,
                                        MPI_COMM_WORLD);
        }
    }

    REPL::REPL()
        : Process(0)
    {}

    void REPL::run()
    {
        // Start repl
        while (true)
        {
            process_msg_from_input();
        }
    }
} // namespace repl
