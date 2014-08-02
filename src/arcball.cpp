#include "arcball.hpp"

Arcball::Arcball(std::shared_ptr<WorldObject> object, std::shared_ptr<Camera> camera, std::shared_ptr<Window> window)
    : allow_constraints(false),
      object(object),
      camera(camera),
      window(window),
      radius(0.75f),
      dragging(false),
      orientation_reset(object->orientation),
      orientation_start(orientation_reset),
      orientation_now(orientation_reset),
      axis_set(AxisSet::NONE),
      axis_index(0)
{
}

void Arcball::update(Input & input)
{
    update_button(input);
    update_key(input);

    drag_arc_from = ball_coord(mouse_position_start);
    drag_arc_to = ball_coord(input.position());

    if (!dragging) {
        update_active_axis_set(input);
        update_constraint_axes();
        axis_index = nearest_constraint_index(drag_arc_to);
    }

    if (dragging) {
        update_drag_arc();
        object->orientation = orientation_now;
    }

    update_result_arc();
}

void Arcball::update_button(Input & input)
{
    const Button drag_button = Button::LEFT;

    dragging = input.down(drag_button);

    if (input.clicked(drag_button)) {
        // begin drag
        mouse_position_start = input.position();
    } else if (input.released(drag_button)) {
        // end drag
        orientation_start = orientation_now;
    }
}

void Arcball::update_key(Input & input)
{
    if (input.pressed(Key::PLUS)) {
        radius = glm::clamp(radius + 0.25f, 0.25f, 1.0f);
    } else if (input.pressed(Key::MINUS)) {
        radius = glm::clamp(radius - 0.25f, 0.25f, 1.0f);
    }

    if (input.pressed(Key::R)) {
        orientation_start = orientation_reset;
        orientation_now = orientation_reset;
        object->orientation = orientation_reset;
    }
}

void Arcball::update_active_axis_set(Input & input)
{
    const bool shift = input.down(Key::LSHIFT);
    const bool ctrl = input.down(Key::LCTRL);

    if (allow_constraints && ctrl && shift) {
        axis_set = AxisSet::WORLD;
    } else if (allow_constraints && ctrl) {
        axis_set = AxisSet::BODY;
    } else if (allow_constraints && shift) {
        axis_set = AxisSet::CAMERA;
    } else {
        axis_set = AxisSet::NONE;
    }
}

void Arcball::update_constraint_axes()
{
    axes.clear();

    // the axes that should not rotate on camera axes is multiplied by the
    // inverse/conjugate of the camera orientation to cancel out the camera
    // orientation when dragging
    glm::quat inv = glm::conjugate(camera->orientation);

    switch (axis_set) {
    case AxisSet::BODY:
        axes.push_back(inv * object->orientation * X_UNIT);
        axes.push_back(inv * object->orientation * Y_UNIT);
        axes.push_back(inv * object->orientation * Z_UNIT);
        break;
    case AxisSet::CAMERA:
        axes.push_back(X_UNIT);
        axes.push_back(Y_UNIT);
        axes.push_back(Z_UNIT);
        break;
    case AxisSet::WORLD:
        axes.push_back(inv * X_UNIT);
        axes.push_back(inv * Y_UNIT);
        axes.push_back(inv * Z_UNIT);
        break;
    }
}

void Arcball::update_drag_arc()
{
    if (axis_set != AxisSet::NONE) {
        drag_arc_from = constrain_to(drag_arc_from, axes[axis_index]);
        drag_arc_to = constrain_to(drag_arc_to, axes[axis_index]);
    }

    // from the two clicked points on the ball we construct a quaternion
    // that represents the rotation which rotates the ball from the
    // initial point to the end point, the quaternion vector (or rotation
    // axis) is brought into eye space
    float w = glm::dot(drag_arc_from, drag_arc_to);
    glm::vec3 v = camera->orientation * glm::cross(drag_arc_from, drag_arc_to);
    glm::quat orientation_drag(w, v);

    // product of two quaternions give the combination of the rotations
    // they represent
    orientation_now = glm::normalize(orientation_drag * orientation_start);
}

// convert the initial orienation to two points on the ball, this is the
// shortest arc for obtaining the current orientation from its starting
// orientation and is called the result arc
void Arcball::update_result_arc()
{
    const glm::quat q = orientation_start;

    // pick an initial point that is perpendicular to the quaternion vector
    float s = sqrt(q.x * q.x + q.y * q.y);
    if (s == 0.0f) {
        result_arc_from.x = 0.0f;
        result_arc_from.y = 1.0f;
        result_arc_from.z = 0.0f;
    } else {
        result_arc_from.x = -q.y / s;
        result_arc_from.y = q.x / s;
        result_arc_from.z = 0.0f;
    }

    result_arc_to.x = q.w * result_arc_from.x - q.z * result_arc_from.y;
    result_arc_to.y = q.w * result_arc_from.y + q.z * result_arc_from.x;
    result_arc_to.z = q.x * result_arc_from.y - q.y * result_arc_from.x;

    // negate initial ball point for a shorter arc
    if (q.w < 0.0f) {
        result_arc_from.x = -result_arc_from.x;
        result_arc_from.y = -result_arc_from.y;
        result_arc_from.z = 0.0f;
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
    auto dimension = window->dimension();

    // TODO: use viewport where x and y is not assumed to be at (0,0)
    float x = 0.0f;
    float y = 0.0f;
    float w = dimension.first;
    float h = dimension.second;

    return glm::vec3(
        2.0f * (mouse_position.x - x) / w - 1.0f,
        -(2.0f * (mouse_position.y - y) / h - 1.0f),
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
int Arcball::nearest_constraint_index(glm::vec3 ball_point)
{
    float max = -std::numeric_limits<float>::infinity();
    int nearest = 0;

    for (unsigned int i = 0; i < axes.size(); i++) {
        glm::vec3 point_on_plane = constrain_to(ball_point, axes[i]);
        float dot = glm::dot(point_on_plane, ball_point);
        if (dot > max) {
            max = dot;
            nearest = i;
        }
    }

    return nearest;
}
