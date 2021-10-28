#pragma once

#include <memory>

namespace process
{
    class Process
    {
    public:
        Process() = default;

        virtual ~Process() = default;

        // XXX
        // private:
        //    unsigned int rank = 0;
    };
} // namespace process
