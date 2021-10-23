#pragma once

#include <string>
#include "rpc/rpc.hh"

// XXX: maybe put this in rpc
namespace client
{

    // XXX: maybe thinks about hierarchy of different messages
    //
    //          - REPL
    //         |
    // Message - RPC -> Append, Request, ....
    //         |
    //          - Client message
    //
    struct ClientRequest : public rpc::RemoteProcedureCall
    {
    public:
        explicit ClientRequest(const std::string& command,
                unsigned int serial_number, unsigned int client_index);
        explicit ClientRequest(const json& json_obj);


        void apply(Server &server);

    private:
        json serialize_json() const;

        const std::string command;
        const unsigned int serial_number;
        const unsigned int client_index;
    };

}
