#ifndef RUNNER_HPP_INCLUDED
#define RUNNER_HPP_INCLUDED

#include "world.hpp"
#include "window.hpp"

#include <memory>

class Runner {
public:
    int control(std::shared_ptr<Window> window, World & world);
};

#endif
