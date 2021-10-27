#pragma once

namespace repl
{
    enum ServerSpeed
    {
        LOW,
        MEDIUM,
        HIGH
    };

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
