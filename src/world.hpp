#ifndef WORLD_HPP_INCLUDED
#define WORLD_HPP_INCLUDED

#include "clock.hpp"
#include "input.hpp"
#include "window.hpp"

#include <memory>

class World {
public:
    virtual bool create(std::shared_ptr<Window> window);
    virtual void update(seconds delta, seconds elapsed, Input & input);
    virtual void render();
    virtual void destroy();
};

#endif
