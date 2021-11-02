#include <iostream>
#include "client-finish.hh"

#include "raft/server.hh"
#include "client/client.hh"

namespace client
{
    ClientFinish::ClientFinish()
        : ClientMsg(CLIENT_MSG_TYPE::CLIENT_FINISH)
    {}

    void ClientFinish::apply(process::Process& process)
    {
        auto& client = dynamic_cast<client::Client&>(process);
        client.on_client_request(*this);
    }
} // namespace client
