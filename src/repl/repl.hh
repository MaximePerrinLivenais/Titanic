#pragma once

namespace repl
{
    enum ServerSpeed
    {
        LOW = 500,
        MEDIUM = 250,
        HIGH = 0
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
