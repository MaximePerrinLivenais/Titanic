#pragma once

enum ServerStatus
{
    // REPL STATE
    CRASHED,

    FOLLOWER,
    LEADER,
    CANDIDATE
};
