#ifndef ARCBALL_HPP_INCLUDED
#define ARCBALL_HPP_INCLUDED

#include "input.hpp"
#include "spatial.hpp"
#include "viewport.hpp"

#include <memory>

class ArcballHelper;

enum class AxisSet {
    NONE,
    CAMERA,
    BODY,
    WORLD
};

struct Arc {
    glm::vec3 from;
    glm::vec3 to;
};

struct Constraint {
    Constraint();

    AxisSet current;
    std::vector<glm::vec3> available;
    unsigned int nearest;
};

struct Orientation {
    Orientation(glm::quat init);

    glm::quat reset;
    glm::quat start;
    glm::quat now;
};

class Arcball {
    friend ArcballHelper;
public:
    Arcball(
        std::shared_ptr<gst::Spatial> object,
        std::shared_ptr<gst::Spatial> camera,
        gst::Viewport viewport);
    void update(gst::Input & input);

    bool allow_constraints;
private:
    void update_button(gst::Input & input);
    void update_key(gst::Input & input);
    void update_current_axis_set(gst::Input & input);
    void update_constraint_axes();
    void update_drag_arc();
    void update_result_arc();
    glm::vec3 ball_coord(glm::ivec2 mouse_position);
    glm::vec3 window_coord(glm::ivec2 mouse_position);
    glm::vec3 constrain_to(glm::vec3 point, glm::vec3 axis);
    int nearest_constraint(glm::vec3 ball_point);

    std::shared_ptr<gst::Spatial> object;
    std::shared_ptr<gst::Spatial> camera;
    gst::Viewport viewport;

    float radius;
    bool dragging;
    glm::ivec2 mouse_position_start;

    Arc drag;
    Arc result;
    Orientation orientation;
    Constraint constraint;
};

#endif
