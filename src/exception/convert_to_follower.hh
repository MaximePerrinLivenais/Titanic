#pragma once

#include <exception>

struct ConvertToFollower : public std::exception
{
    const char *what() const throw()
    {
        return "ConvertToFollower exception";
    }
};
