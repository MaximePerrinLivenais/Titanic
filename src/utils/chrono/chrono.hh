#pragma once

#include <chrono>

namespace chrono
{
    using high_resolution_clock = std::chrono::high_resolution_clock;
    using milliseconds = std::chrono::milliseconds;

    using time_point = std::chrono::time_point<high_resolution_clock>;

    unsigned long get_time_milliseconds();
} // namespace chrono
