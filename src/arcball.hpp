#ifndef ARCBALL_HPP_INCLUDED
#define ARCBALL_HPP_INCLUDED

#include "camera.hpp"
#include "clock.hpp"
#include "key.hpp"
#include "window.hpp"
#include "worldobject.hpp"

enum class AxisSet {
    NONE,
    CAMERA,
    BODY,
    WORLD
};

class Arcball {
    friend class ArcballHelper;
public:
    Arcball(std::shared_ptr<WorldObject> object, std::shared_ptr<Camera> camera, std::shared_ptr<Window> window);
    void update(Input & input);

    bool allow_constraints;
private:
    void update_button(Input & input);
    void update_key(Input & input);
    void update_active_axis_set(Input & input);
    void update_constraint_axes();
    void update_drag_arc();
    void update_result_arc();

    glm::vec3 ball_coord(glm::ivec2 mouse_position);
    glm::vec3 window_coord(glm::ivec2 mouse_position);

    glm::vec3 constrain_to(glm::vec3 point, glm::vec3 axis);
    int nearest_constraint_index(glm::vec3 ball_point);

    std::shared_ptr<WorldObject> object;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Window> window;

    float radius;

    bool dragging;

    glm::ivec2 mouse_position_start;

    glm::vec3 drag_arc_from;
    glm::vec3 drag_arc_to;

    glm::vec3 result_arc_from;
    glm::vec3 result_arc_to;

    glm::quat orientation_reset;
    glm::quat orientation_start;
    glm::quat orientation_now;

    AxisSet axis_set;
    unsigned int axis_index;
    std::vector<glm::vec3> axes;
};

#endif
