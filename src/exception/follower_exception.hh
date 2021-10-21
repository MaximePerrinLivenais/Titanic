#pragma once

#include <exception>

struct FollowerException : public std::exception
{
    const char *what() const throw()
    {
        return "ConvertToFollower exception";
    }
};
