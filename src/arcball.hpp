#ifndef ARCBALL_HPP_INCLUDED
#define ARCBALL_HPP_INCLUDED

#include "gust.hpp"

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
    AxisSet current;
    std::vector<glm::vec3> available;
    unsigned int nearest;
};

struct Orientation {
    glm::quat reset;
    glm::quat start;
    glm::quat now;
};

// The responsibility of this class is to manipulate a spatial object with a
// virtual arcball.
class Arcball {
    friend ArcballHelper;
public:
    // Construct empty arcball.
    Arcball() = default;
    // Construct arcball with a object to be manipulated.
    Arcball(std::shared_ptr<gst::Spatial> object);
    // Update arcball from specified input, eye and viewport.
    void update(
        gst::Input const & input,
        gst::CameraNode const & eye,
        gst::Viewport const & viewport);
    // Set enable/disable if object can be locked and manipulated on a
    // specific axis.
    void set_allow_constraints(bool allow_constraints);
private:
    void update_button(gst::Input const & input);
    void update_key(gst::Input const & input);
    void update_current_axis_set(gst::Input const & input);
    void update_constraint_axes(gst::CameraNode const & eye);
    void update_drag_arc(gst::CameraNode const & eye);
    void update_result_arc();

    glm::vec3 ball_coord(gst::Viewport const & viewport, glm::ivec2 mouse_position);
    glm::vec3 window_coord(gst::Viewport const & viewport, glm::ivec2 mouse_position);
    glm::vec3 constrain_to(glm::vec3 point, glm::vec3 axis);
    int nearest_constraint(glm::vec3 ball_point);

    std::shared_ptr<gst::Spatial> object;

    bool allow_constraints;
    float radius;
    bool dragging;
    glm::ivec2 mouse_position_start;

    Arc drag;
    Arc result;
    Orientation orientation;
    Constraint constraint;
};

#endif
