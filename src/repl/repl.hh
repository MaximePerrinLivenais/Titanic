#pragma once

namespace repl
{
    class REPL
    {
    public:
        REPL() = default;

        void run();

        // void send_crash_request();

    private:
        // XXX: List of Process (Server + client)
    };
} // namespace repl
