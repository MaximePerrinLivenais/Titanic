#include "append-entries-response.hh"
#include "raft/server.hh"

namespace rpc
{
    AppendEntriesResponse::AppendEntriesResponse(const unsigned int term, const bool success,
            const unsigned int follower_index)
        : RemoteProcedureCall(term, RPC_TYPE::APPEND_ENTRIES_RESPONSE), success(success),
            follower_index(follower_index)
    {}

    AppendEntriesResponse::AppendEntriesResponse(const json& json_obj)
        : AppendEntriesResponse(json_obj["term"], json_obj["success"],
                json_obj["follower_index"])
    {}

    void AppendEntriesResponse::apply(Server& server)
    {
        server.on_append_entries_response(*this);
    }

    bool AppendEntriesResponse::get_success() const
    {
        return success;
    }

    unsigned int AppendEntriesResponse::get_follower_index() const
    {
        return follower_index;
    }

    json AppendEntriesResponse::serialize_json() const
    {
        json serialization = json();

        serialization["success"] = success;
        serialization["follower_index"] = follower_index;

        return serialization;
    }
}
