#include "chrono.hh"

namespace chrono
{
    unsigned long get_time_milliseconds()
    {
        auto time_point = high_resolution_clock::now().time_since_epoch();
        return std::chrono::duration_cast<milliseconds>(time_point).count();
    }
} // namespace chrono
