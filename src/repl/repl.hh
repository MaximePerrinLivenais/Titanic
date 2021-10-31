#pragma once

#include "process/process.hh"

namespace repl
{
    class REPL : public process::Process
    {
    public:
        REPL();

        void run();
    };
} // namespace repl
