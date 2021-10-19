#include "rpc/rpc.hh"

using namespace rpc;

RemoteProcedureCall::RemoteProcedureCall(const int term)
    : term(term)
{}

int RemoteProcedureCall::get_term() const
{
    return term;
}
