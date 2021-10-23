#include "client-request.hh"

namespace client
{
    ClientRequest::ClientRequest(const std::string& command,
                unsigned int serial_number, unsigned int client_index):
        rpc::RemoteProcedureCall(-1, rpc::RPC_TYPE::CLIENT_REQUEST),
        command(command), serial_number(serial_number), client_index(client_index)
    {
    }


    ClientRequest::ClientRequest(const json& json_obj):
        ClientRequest(json_obj["command"], json_obj["serial_number"], json_obj["client_index"])
    {
    }


    json ClientRequest::serialize_json() const
    {
        json serialization = json();

        serialization["command"] = command;
        serialization["serial_number"] = serial_number;
        serialization["client_index"] = client_index;

        return serialization;
    }

    void ClientRequest::apply(Server &server)
    {
        (void)server;
        throw std::logic_error("Function not yet implemented");
    }

}
