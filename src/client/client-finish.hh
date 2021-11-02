#pragma once

#include "client/client-message.hh"

namespace client
{
    class ClientFinish : public ClientMsg
    {
    public:
        explicit ClientFinish();

        void apply(process::Process& process) final;
    };

    using shared_client_finish = std::shared_ptr<ClientFinish>;

} // namespace client
