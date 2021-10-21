#pragma once

#include "rpc.hh"

namespace rpc
{
    class AppendEntriesResponse : public RemoteProcedureCall
    {
        public:
            explicit AppendEntriesResponse(const unsigned int term, const bool success);

            explicit AppendEntriesResponse(const json& json_obj);

            void apply(Server& server);

            bool get_success() const;

        private:
            json serialize_json() const;

            const bool success;
    };
}
