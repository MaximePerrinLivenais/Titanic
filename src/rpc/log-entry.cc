#include "log-entry.hh"

namespace rpc
{
    LogEntry::LogEntry(const std::string command, const unsigned int term)
        : command(command)
        , term(term)
    {}
} // namespace rpc
