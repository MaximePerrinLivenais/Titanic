#pragma once

namespace process
{
    class Process
    {
    public:
        Process(const unsigned int rank);

        virtual ~Process() = default;

    protected:
        const unsigned int rank;
    };
} // namespace process
