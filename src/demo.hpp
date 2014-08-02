#ifndef DEMO_HPP_INCLUDED
#define DEMO_HPP_INCLUDED

#include "arcball.hpp"
#include "arcballhelper.hpp"
#include "mesh.hpp"
#include "orthocamera.hpp"
#include "perspectivecamera.hpp"
#include "program.hpp"
#include "world.hpp"

class Demo : public World {
public:
    Demo();
    bool create(std::shared_ptr<Window> window);
    void update(seconds delta, seconds elapsed, Input & input);
    void render();
    void destroy();
private:
    void render_model();
    void render_axes();
    void render_helper();

    std::shared_ptr<Window> window;
    int width;
    int height;

    Program light_program;
    Program basic_program;

    std::shared_ptr<PerspectiveCamera> camera;
    std::shared_ptr<Arcball> arcball;
    std::shared_ptr<WorldObject> model;

    glm::vec4 light_direction;

    ArcballHelper helper;
    OrthoCamera helper_camera;

    Mesh axes;

    bool show_helpers;
};

#endif
