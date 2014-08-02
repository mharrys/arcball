#ifndef CLOCK_HPP_INCLUDED
#define CLOCK_HPP_INCLUDED

#include <chrono>

typedef std::chrono::duration<float> seconds;
typedef std::chrono::high_resolution_clock::time_point time_point;

class Clock {
public:
    Clock();
    seconds elapsed();
    seconds delta();
private:
    time_point start;
    time_point last;
};

#endif
