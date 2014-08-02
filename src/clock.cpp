#include "clock.hpp"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;

Clock::Clock()
    : start(high_resolution_clock::now()),
      last(start)
{
}

seconds Clock::elapsed()
{
    auto current = high_resolution_clock::now();
    auto elapsed = current - start;

    return duration_cast<seconds>(elapsed);
}

seconds Clock::delta()
{
    auto current = high_resolution_clock::now();
    auto delta = current - last;
    last = current;

    // prevent unmanageable delta values
    auto max_delta = seconds(1.0f);
    if (delta > max_delta) {
        return max_delta;
    }

    return duration_cast<seconds>(delta);
}
