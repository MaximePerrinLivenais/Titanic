#include "client-request.hh"

namespace client
{
    ClientRequest::ClientRequest(const std::string& command,
            const std::string& serial_number, unsigned int client_index):
        rpc::RemoteProcedureCall(-1, rpc::RPC_TYPE::CLIENT_REQUEST),
        command(command), serial_number(serial_number), client_index(client_index)
    {
    }


}
