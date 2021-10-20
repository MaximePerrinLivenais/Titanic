#include "chrono.hh"

#include <chrono>

using namespace chrono;

using high_resolution_clock = std::chrono::high_resolution_clock;
using milliseconds = std::chrono::milliseconds;

unsigned long get_time_milliseconds()
{
    auto time_point = high_resolution_clock::now().time_since_epoch();
    return std::chrono::duration_cast<milliseconds>(time_point).count();
}
