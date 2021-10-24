#pragma once

#include <memory>
#include <string>

#include "misc/json.hh"

class Server;

namespace message
{
    class Message;

    using shared_msg = std::shared_ptr<Message>;

    enum MSG_TYPE
    {
        RPC_MESSAGE,
        REPL_MESSAGE,
    };

    class Message
    {
    public:
        explicit Message(const MSG_TYPE msg_type);
        virtual ~Message() = default;

        MSG_TYPE get_msg_type() const;

        virtual const std::string serialize() const;
        static shared_msg deserialize(const std::string& message);

        virtual void apply_message(Server& server) = 0;

    private:
        virtual json serialize_json() const = 0;

        const MSG_TYPE msg_type;
    };

} // namespace message
