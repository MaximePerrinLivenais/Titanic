#pragma once

namespace rpc
{
    class RemoteProcedureCall
    {
        public:
            explicit RemoteProcedureCall(const int term);

            int get_term() const;

        private:
            const int term;
    };
}
