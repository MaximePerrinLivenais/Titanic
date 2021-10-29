#pragma once

#include <memory>

#include "message/message.hh"

using namespace message;

namespace repl
{
    enum REPL_MSG_TYPE
    {
        SPEED,
        CRASH,
        START,
    };

    class ReplMsg;

    using shared_repl_msg = std::shared_ptr<ReplMsg>;

    class ReplMsg : public Message
    {
    public:
        explicit ReplMsg(const REPL_MSG_TYPE repl_msg_type);

        REPL_MSG_TYPE get_repl_msg_type() const;

        static shared_repl_msg deserialize(const std::string& message);

        virtual void send() = 0;

    protected:
        virtual json serialize_json() const = 0;

        const REPL_MSG_TYPE repl_msg_type;
    };
} // namespace repl
