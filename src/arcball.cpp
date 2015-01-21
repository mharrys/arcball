#include "arcball.hpp"

Constraint::Constraint()
    : current(AxisSet::NONE),
      nearest(0)
{
}

Orientation::Orientation(glm::quat init)
    : reset(init),
      start(init),
      now(init)
{
}

Arcball::Arcball(
    std::shared_ptr<gst::Spatial> object,
    std::shared_ptr<gst::Spatial> camera,
    gst::Viewport viewport)
    : allow_constraints(false),
      object(object),
      camera(camera),
      viewport(viewport),
      radius(0.75f),
      dragging(false),
      orientation(object->orientation)
{
}

void Arcball::update(gst::Input & input)
{
    update_button(input);
    update_key(input);

    drag.from = ball_coord(mouse_position_start);
    drag.to = ball_coord(input.position());

    if (!dragging) {
        update_current_axis_set(input);
        update_constraint_axes();
        constraint.nearest = nearest_constraint(drag.to);
    }

    if (dragging) {
        update_drag_arc();
        object->orientation = orientation.now;
    }

    update_result_arc();
}

void Arcball::update_button(gst::Input & input)
{
    const auto drag_button = gst::Button::LEFT;

    dragging = input.down(drag_button);

    if (input.clicked(drag_button)) {
        // begin drag
        mouse_position_start = input.position();
    } else if (input.released(drag_button)) {
        // end drag
        orientation.start = orientation.now;
    }
}

void Arcball::update_key(gst::Input & input)
{
    if (input.pressed(gst::Key::PLUS)) {
        radius = glm::clamp(radius + 0.25f, 0.25f, 1.0f);
    } else if (input.pressed(gst::Key::MINUS)) {
        radius = glm::clamp(radius - 0.25f, 0.25f, 1.0f);
    }

    if (input.pressed(gst::Key::R)) {
        orientation.start = orientation.reset;
        orientation.now = orientation.reset;
        object->orientation = orientation.reset;
    }
}

void Arcball::update_current_axis_set(gst::Input & input)
{
    const bool shift = input.down(gst::Key::LSHIFT);
    const bool ctrl = input.down(gst::Key::LCTRL);

    if (allow_constraints && ctrl && shift) {
        constraint.current = AxisSet::WORLD;
    } else if (allow_constraints && ctrl) {
        constraint.current = AxisSet::BODY;
    } else if (allow_constraints && shift) {
        constraint.current = AxisSet::CAMERA;
    } else {
        constraint.current = AxisSet::NONE;
    }
}

void Arcball::update_constraint_axes()
{
    constraint.available.clear();

    // the axes that should not rotate on camera axes is multiplied by the
    // inverse/conjugate of the camera orientation to cancel out the camera
    // orientation when dragging
    glm::quat inv = glm::conjugate(camera->orientation);

    switch (constraint.current) {
    case AxisSet::BODY:
        constraint.available = {
            inv * object->orientation * X_UNIT,
            inv * object->orientation * Y_UNIT,
            inv * object->orientation * Z_UNIT
        };
        break;
    case AxisSet::CAMERA:
        constraint.available = {
            X_UNIT,
            Y_UNIT,
            Z_UNIT
        };
        break;
    case AxisSet::WORLD:
        constraint.available = {
            inv * X_UNIT,
            inv * Y_UNIT,
            inv * Z_UNIT
        };
        break;
    case AxisSet::NONE:
        break;
    }
}

void Arcball::update_drag_arc()
{
    if (constraint.current != AxisSet::NONE) {
        drag.from = constrain_to(drag.from, constraint.available[constraint.nearest]);
        drag.to = constrain_to(drag.to, constraint.available[constraint.nearest]);
    }

    // from the two clicked points on the ball we construct a quaternion
    // that represents the rotation which rotates the ball from the
    // initial point to the end point, the quaternion vector (or rotation
    // axis) is brought into eye space
    float w = glm::dot(drag.from, drag.to);
    glm::vec3 v = camera->orientation * glm::cross(drag.from, drag.to);
    glm::quat orientation_drag(w, v);

    // product of two quaternions give the combination of the rotations
    // they represent
    orientation.now = glm::normalize(orientation_drag * orientation.start);
}

// convert the initial orienation to two points on the ball, this is the
// shortest arc for obtaining the current orientation from its starting
// orientation and is called the result arc
void Arcball::update_result_arc()
{
    const glm::quat q = orientation.start;

    // pick an initial point that is perpendicular to the quaternion vector
    float s = sqrt(q.x * q.x + q.y * q.y);
    if (s == 0.0f) {
        result.from.x = 0.0f;
        result.from.y = 1.0f;
        result.from.z = 0.0f;
    } else {
        result.from.x = -q.y / s;
        result.from.y = q.x / s;
        result.from.z = 0.0f;
    }

    result.to.x = q.w * result.from.x - q.z * result.from.y;
    result.to.y = q.w * result.from.y + q.z * result.from.x;
    result.to.z = q.x * result.from.y - q.y * result.from.x;

    // negate initial ball point for a shorter arc
    if (q.w < 0.0f) {
        result.from.x = -result.from.x;
        result.from.y = -result.from.y;
        result.from.z = 0.0f;
    }
}

// return coordinate on the ball from mouse position
glm::vec3 Arcball::ball_coord(glm::ivec2 mouse_position)
{
    glm::vec3 window_mouse_position = window_coord(mouse_position);

    // TODO: center should be at the object position and in window coordinates
    glm::vec3 center = glm::vec3(0.0f);
    glm::vec3 point = (window_mouse_position - center) / radius;

    float r = glm::length2(point);
    if (r > 1.0f) {
        // set to nearest point on ball
        point *= (1.0f / sqrt(r));
    } else {
        // point on ball
        point.z = sqrt(1.0f - r);
    }

    return point;
}

// return window coordinate from mouse position
glm::vec3 Arcball::window_coord(glm::ivec2 mouse_position)
{
    return glm::vec3(
        2.0f * (mouse_position.x - viewport.get_x()) / viewport.get_width() - 1.0f,
        -(2.0f * (mouse_position.y - viewport.get_y()) / viewport.get_height() - 1.0f),
        0.0f
    );
}

// return specified ball point constrained to specified axis by projecting the
// ball point onto a perpendicular plane relative to the constraint axis, the
// ball point is also flipped to the front when necessary
glm::vec3 Arcball::constrain_to(glm::vec3 point, glm::vec3 axis)
{
    glm::vec3 point_on_plane;
    glm::vec3 proj = point - (axis * glm::dot(axis, point));

    float length = glm::length(proj);
    if (length > 0.0f) {
        float s = 1.0f / length;
        if (proj.z < 0.0f) {
            s = -s;
        }
        point_on_plane = proj * s;
    } else if (axis.z == 1.0f) {
        point_on_plane = glm::vec3(1.0f, 0.0f, 0.0f);
    } else {
        point_on_plane = glm::normalize(glm::vec3(-axis.y, axis.x, 0.0f));
    }

    return point_on_plane;
}

// return index for nearest arc relative to specified point
int Arcball::nearest_constraint(glm::vec3 ball_point)
{
    float max = -std::numeric_limits<float>::infinity();
    int nearest = 0;

    for (unsigned int i = 0; i < constraint.available.size(); i++) {
        glm::vec3 point_on_plane = constrain_to(ball_point, constraint.available[i]);
        float dot = glm::dot(point_on_plane, ball_point);
        if (dot > max) {
            max = dot;
            nearest = i;
        }
    }

    return nearest;
}
