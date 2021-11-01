#include "repl.hh"

#include <set>
#include <iostream>
#include <sstream>
#include <string>

#include "request-crash-repl.hh"
#include "request-start-repl.hh"
#include "request-speed-repl.hh"
#include "request-send-repl.hh"
#include "request-recovery-repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{

    bool is_valid_command(const std::string& command)
    {
        std::set<std::string> valid_command = {
            "CRASH",
            "RECOVERY",
            "SEND",
            "SPEED",
            "START"
        };

        return valid_command.contains(command);
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

    static std::optional<shared_repl_msg> build_message_from_input(unsigned int nb_process)
    {
        //std::cout << "Enter command:\n";

        std::string command_name;
        std::cin >> command_name;

        if (!is_valid_command(command_name))
        {
            if (command_name.size())
                std::clog << command_name << ": Unknown command\n";
            return std::nullopt;
        }

        unsigned int process_target;
        std::cin >> process_target;
        if (process_target < 1 || process_target >= nb_process)
        {
            std::clog << "Invalid rank : " << process_target << ". Max is "
                << nb_process - 1 << "\n";
            return std::nullopt;
        }


        if (command_name == "CRASH")
            return std::make_shared<RequestCrashREPL>(process_target);
        else if (command_name == "SPEED")
        {
            std::string speed_str;
            std::cin >> speed_str;
            auto speed = str_to_server_speed(speed_str);
            if (!speed.has_value())
            {
                std::cout << "Invalid speed option: " << speed_str << "\n";
                return std::nullopt;
            }

            return std::make_shared<RequestSpeedREPL>(process_target, speed.value());
        }
        else if (command_name == "START")
            return std::make_shared<RequestStartREPL>(process_target);
        else if (command_name == "SEND")
            return std::make_shared<RequestSendREPL>(process_target);
        else if (command_name == "RECOVERY")
            return std::make_shared<RequestRecoveryREPL>(process_target);

        return std::nullopt;
    }

    void REPL::run()
    {
        unsigned int nb_process = mpi::MPI_Get_group_comm_size(MPI_COMM_WORLD);

        // Start repl
        while (true)
        {
            auto msg = build_message_from_input(nb_process);
            if (msg.has_value())
            {
                msg.value()->send();
            }
        }
    }
} // namespace repl
