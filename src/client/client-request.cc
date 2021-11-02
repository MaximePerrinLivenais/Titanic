#include <iostream>
#include "client-request.hh"

#include "raft/server.hh"
#include "client/client.hh"

namespace client
{
    ClientRequest::ClientRequest(const std::string& command,
                                 unsigned int serial_number,
                                 unsigned int client_index)
        : ClientMsg(CLIENT_MSG_TYPE::CLIENT_REQUEST)
        , command(command)
        , serial_number(serial_number)
        , client_index(client_index)
    {}

    ClientRequest::ClientRequest(const json& json_obj)
        : ClientRequest(json_obj["command"], json_obj["serial_number"],
                        json_obj["client_index"])
    {}

    void ClientRequest::apply(process::Process& process)
    {
        auto& server = dynamic_cast<raft::Server&>(process);
        server.on_client_request(*this);
    }

    json ClientRequest::serialize_json() const
    {
        json serialization = ClientMsg::serialize_json();

        serialization["command"] = command;
        serialization["serial_number"] = serial_number;
        serialization["client_index"] = client_index;

        return serialization;
    }

    std::string ClientRequest::get_command() const
    {
        return command;
    }

    unsigned int ClientRequest::get_serial_number() const
    {
        return serial_number;
    }

    unsigned int ClientRequest::get_client_index() const
    {
        return client_index;
    }
} // namespace client
