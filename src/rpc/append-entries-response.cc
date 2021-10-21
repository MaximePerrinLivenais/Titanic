#include "append-entries-response.hh"

namespace rpc
{
    AppendEntriesResponse::AppendEntriesResponse(const unsigned int term, const bool success)
        : RemoteProcedureCall(term, RPC_TYPE::APPEND_ENTRIES_RESPONSE), success(success)
    {}

    AppendEntriesResponse::AppendEntriesResponse(const json& json_obj)
        : AppendEntriesResponse(json_obj["term"], json_obj["success"])
    {}

    void AppendEntriesResponse::apply(Server& server)
    {
        (void) server;
    }

    bool AppendEntriesResponse::get_success() const
    {
        return success;
    }

    json AppendEntriesResponse::serialize_json() const
    {
        json serialization = json();

        serialization["success"] = success;

        return serialization;
    }
}
