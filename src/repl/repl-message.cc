#include "repl-message.hh"

namespace repl
{
    ReplMsg::ReplMsg(const REPL_MSG_TYPE repl_type)
        : Message(MSG_TYPE::REPL_MESSAGE)
        , repl_msg_type(repl_type)
    {}

    void ReplMsg::apply_message(Server& server)
    {
        this->apply(server);
    }

    const std::string ReplMsg::serialize() const
    {
        json serialization = this->serialize_json();

        serialization["repl_msg_type"] = repl_msg_type;

        return serialization.dump(4);
    }

    static shared_repl_msg deserialize(const std::string& message)
    {
        auto json_obj = json::parse(message);
        auto repl_msg_type = json_obj["repl_msg_type"].get<REPL_MSG_TYPE>();

        // TODO construct right object here
        switch (repl_msg_type)
        {
        case SPEED:
            abort();
        case CRASH:
            abort();
        case START:
            abort();
        }

        throw std::invalid_argument("Not corresponding to existing RPCs");
    }

    REPL_MSG_TYPE ReplMsg::get_repl_msg_type() const
    {
        return repl_msg_type;
    }

} // namespace repl
