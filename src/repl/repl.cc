#include "repl.hh"

#include <set>
#include <iostream>
#include <sstream>
#include <string>

#include "request-crash-repl.hh"
#include "request-speed-repl.hh"
#include "request-send-repl.hh"
#include "request-recovery-repl.hh"
#include "request-start-repl.hh"
#include "server-speed.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{

    REPL::REPL()
        : Process(0)
    {}

    static std::optional<shared_repl_msg> process_request(std::string& command)
    {
        if (command == "CRASH")
            return std::make_shared<RequestCrashREPL>();
        else if (command == "SPEED")
        {
            // Handle speed message
            std::string speed_str;
            std::cin >> speed_str;

            auto speed = str_to_server_speed(speed_str);
            if (!speed.has_value())
            {
                std::cout << "Invalid speed option: " << speed_str << "\n";
                return std::nullopt;
            }
            return std::make_shared<RequestSpeedREPL>(speed.value());
        }
        else if (command == "SEND")
            return std::make_shared<RequestSendREPL>();
        else if (command == "RECOVERY")
            return std::make_shared<RequestRecoveryREPL>();
        else if (command == "START")
            return std::make_shared<RequestStartREPL>();

        return std::nullopt;
    }

    static void process_msg_from_input(unsigned nb_process)
    {
        std::string command_name;
        std::cin >> command_name;

        std::optional<shared_repl_msg> repl_msg = process_request(command_name);

        if (repl_msg.has_value())
        {
            unsigned target_process = 0;
            std::cin >> target_process;

            if (target_process < 1 || target_process >= nb_process)
            {
                std::cout << "Invalid rank : " << target_process  << ". Max is "
                    << nb_process - 1 << "\n";
                return;
            }

            mpi::MPI_Serialize_and_send(repl_msg.value(), target_process, 0,
                                        MPI_COMM_WORLD);
        }
    }

    void REPL::run()
    {
        unsigned int nb_process = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);

        while (true)
            process_msg_from_input(nb_process);
    }


    std::optional<ServerSpeed> str_to_server_speed(const std::string& speed_str)
    {
        if (speed_str == "LOW")
            return LOW;
        else if (speed_str == "MEDIUM")
            return MEDIUM;
        else if (speed_str == "HIGH")
            return HIGH;

        return std::nullopt;
    }
} // namespace repl
