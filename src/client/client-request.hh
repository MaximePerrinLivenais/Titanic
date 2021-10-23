#pragma once

#include <string>
#include "rpc/rpc.hh"

// XXX: maybe put this in rpc
namespace client
{
    struct ClientRequest : public rpc::RemoteProcedureCall
    {
    public:
        ClientRequest(const std::string& command,
                const std::string& serial_number, unsigned int client_index);



    private:
        const std::string command;
        const std::string serial_number;
        const unsigned int client_index;
    };

}
