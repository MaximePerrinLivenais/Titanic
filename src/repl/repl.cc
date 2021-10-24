#include "repl.hh"

#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "request-crash-repl.hh"
#include "request-start-repl.hh"
#include "utils/openmpi/mpi-wrapper.hh"

namespace repl
{
    static std::optional<shared_repl_msg> build_message_from_input()
    {
        //std::cout << "Enter command:\n";

        std::string command_name;
        std::cin >> command_name;

        if (command_name == "CRASH")
        {
            unsigned int process_target;
            std::cin >> process_target;
            return std::make_shared<RequestCrashREPL>(process_target);
        }
        else if (command_name == "SPEED")
        {
            std::clog << "Not implemented yet\n";
        }
        else if (command_name == "START")
        {
            unsigned int process_target;
            std::cin >> process_target;
            return std::make_shared<RequestStartREPL>(process_target);
        }
        else
        {
            std::clog << command_name << ": Unknown command\n";
        }

        return std::nullopt;
    }

    void REPL::run()
    {
        // Start repl
        while (true)
        {
            auto msg = build_message_from_input();

            if (msg.has_value())
            {
                msg.value()->send();
            }
        }
    }
} // namespace repl
