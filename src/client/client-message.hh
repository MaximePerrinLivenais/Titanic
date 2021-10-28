#pragma once

#include "message/message.hh"

namespace client
{
    enum CLIENT_MSG_TYPE
    {
        CLIENT_REQUEST,
        CLIENT_RESPONSE
    };

    class ClientMsg;
    using shared_client_msg = std::shared_ptr<ClientMsg>;

    class ClientMsg : public message::Message
    {
    public:
        explicit ClientMsg(const CLIENT_MSG_TYPE client_msg_type);

        CLIENT_MSG_TYPE get_client_msg_type() const;

        static shared_client_msg deserialize(const std::string& message);

        void apply_message(Process& process) final;

    protected:
        virtual json serialize_json() const = 0;
        virtual void apply(Server& server) = 0;

        const CLIENT_MSG_TYPE client_msg_type;
    };

} // namespace client
