#pragma once

namespace process
{
    class Process
    {
    public:
        Process(int rank);

        virtual ~Process() = default;

    protected:
        int rank;
    };
} // namespace process
