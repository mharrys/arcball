#include "runner.hpp"

#include "clock.hpp"
#include "input.hpp"

#include <iostream>

int Runner::control(std::shared_ptr<Window> window, World & world)
{
    if (!window->is_open()) {
        std::cerr << "Runner::control: window has not been opened." << std::endl;
        return 1;
    }

    if (!world.create(window)) {
        std::cerr << "Runner::control: could not created world without errors." << std::endl;
        return 1;
    }

    Clock clock;
    Input input;
    while (!window->should_close()) {
        window->poll(input);

        world.update(clock.delta(), clock.elapsed(), input);
        world.render();

        window->swap();
    }

    return 0;
}
