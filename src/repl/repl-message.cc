#include "repl-message.hh"

#include "request-crash-repl.hh"
#include "request-start-repl.hh"
#include "request-speed-repl.hh"

// TODO remove header
#include <iostream>

namespace repl
{
    ReplMsg::ReplMsg(const REPL_MSG_TYPE repl_type)
        : Message(MSG_TYPE::REPL_MESSAGE)
        , repl_msg_type(repl_type)
    {}

    void ReplMsg::apply_message(Server& server)
    {
        std::cout << "I apply a repl message\n";
        this->apply(server);
    }

    json ReplMsg::serialize_json() const
    {
        json serialization = json();

        serialization["repl_msg_type"] = repl_msg_type;

        return serialization;
    }

    shared_repl_msg ReplMsg::deserialize(const std::string& message)
    {
        auto json_obj = json::parse(message);
        auto repl_msg_type = json_obj["repl_msg_type"].get<REPL_MSG_TYPE>();

        // TODO construct right object here
        switch (repl_msg_type)
        {
        case SPEED:
            return std::make_shared<RequestSpeedREPL>(json_obj);
        case CRASH:
            return std::make_shared<RequestCrashREPL>(json_obj);
        case START:
            return std::make_shared<RequestStartREPL>(json_obj);
        }

        throw std::invalid_argument("Not corresponding to existing REPL message");
    }

    REPL_MSG_TYPE ReplMsg::get_repl_msg_type() const
    {
        return repl_msg_type;
    }

} // namespace repl
