#pragma once

#include <memory>

#include "client/client-message.hh"
#include "rpc/rpc.hh"

namespace process
{
    class Process
    {
    public:
        Process() = default;

        virtual ~Process() = default;
    };
} // namespace process
