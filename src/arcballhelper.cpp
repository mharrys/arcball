#include "arcballhelper.hpp"


ArcballHelper::ArcballHelper()
    : show_drag(true),
      show_constraints(true),
      show_result(true),
      show_rim(true),
      override_rim(false)
{
    // draws current dragging arc from two points on the ball
    drag.mode = DrawMode::LINE_STRIP;
    drag.usage = DataUsage::DYNAMIC;

    // draws a circle of the dragging area
    rim.mode = DrawMode::LINE_LOOP;
    rim.usage = DataUsage::DYNAMIC;
    rim.opacity = 0.4f;

    // draws resulting (shortest) arc for reaching the current orientation
    result.usage = DataUsage::DYNAMIC;

    // draws axis constraints and highlight the nearest axis in the axis set
    // relative to the mouse position
    for (auto & constraint : constraints) {
        constraint.mode = DrawMode::LINE_STRIP;
        constraint.usage = DataUsage::DYNAMIC;
    }
}

void ArcballHelper::target(std::shared_ptr<Arcball> arcball)
{
    this->arcball = arcball;
}

void ArcballHelper::update()
{
    if (!arcball) {
        return;
    }

    update_drag();
    update_constraints();
    update_result();
    update_rim();
}

void ArcballHelper::draw(Program & program, Camera & camera)
{
    program.use();

    glm::mat4 v = camera.view();
    glm::mat4 p = camera.projection();
    glm::mat4 mvp = p * v;
    program.set_uniform("mvp", mvp);

    auto draw_mesh = [& program](Mesh & mesh) {
        program.set_uniform("use_mesh_color", mesh.use_color);
        program.set_uniform("mesh_color", mesh.color);
        program.set_uniform("mesh_opacity", mesh.opacity);

        mesh.draw();
    };

    if (show_drag) {
        draw_mesh(drag);
    }

    if (show_rim) {
        draw_mesh(rim);
    }

    if (show_constraints) {
        for (auto & constraint : constraints) {
            draw_mesh(constraint);
        }
    }

    if (show_result) {
        draw_mesh(result);
    }
}

void ArcballHelper::update_drag()
{
    drag.clear();

    if (arcball->dragging) {
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.0f);

        // set appropiate drag arc color depending on the constraint axis
        if (arcball->axis_set != AxisSet::NONE) {
            color = axis_index_color(arcball->axis_index);
        }

        fill_arc(drag, arcball->drag_arc_from, arcball->drag_arc_to, color);
    }

    drag.update_positions = true;
    drag.update_colors = true;
}

void ArcballHelper::update_rim()
{
    rim.clear();

    if (!override_rim) {
        glm::vec3 gray(0.3f, 0.3f, 0.3f);
        fill_circle(rim, gray);
    }

    rim.update_positions = true;
    rim.update_colors = true;
}

void ArcballHelper::update_constraints()
{
    for (auto & constraint : constraints) {
        constraint.clear();
        constraint.update_positions = true;
        constraint.update_colors = true;
    }

    if (!arcball->allow_constraints) {
        return;
    }

    override_rim = false;
    if (arcball->dragging) {
        // show only focus axis
        if (arcball->axis_set != AxisSet::NONE) {
            fill_constraint(arcball->axis_index);
            // we use points to "fill" the axis with the drag arc line
            constraints[arcball->axis_index].mode = DrawMode::POINTS;
        }
    } else {
        // show all available axes and highlight the nearest axis
        if (arcball->axis_set != AxisSet::NONE) {
            auto & axes = arcball->axes;
            for (unsigned int i = 0; i < axes.size(); i++) {
                fill_constraint(i);
            }
        }
    }
}

void ArcballHelper::update_result()
{
    result.clear();

    glm::vec3 orange(1.0f, 0.5f, 0.0f);
    fill_arc(result, arcball->result_arc_from, arcball->result_arc_to, orange);
}

void ArcballHelper::fill_constraint(unsigned int index)
{
    auto & constraint = constraints[index];
    constraint.mode = DrawMode::LINE_STRIP;
    constraint.opacity = arcball->axis_index == index ? 1.0f : 0.4f;

    auto axis = arcball->axes[index];
    auto color = axis_index_color(index);

    if (axis.z == 1.0f) {
        // we are looking down through the z-axis
        constraint.mode = DrawMode::LINE_LOOP;
        fill_circle(constraint, color);
        // we signal to not draw our rim to avoid color conflicts when drawing
        override_rim = true;
    } else {
        fill_half_arc(constraint, axis, color);
    }
}

void ArcballHelper::fill_circle(Mesh & helper, glm::vec3 color)
{
    const int segments = 64;
    const float radius = arcball->radius;
    const float PI_2 = PI * 2.0f;
    const float segment = PI_2 / segments;

    for (float i = 0.0f; i < PI_2; i += segment) {
        glm::vec3 position(cos(i), sin(i), 0.0f);
        helper.positions.push_back(position * radius);
        if (!helper.use_color) {
            helper.colors.push_back(color);
        } else {
            helper.color = color;
        }
    }
}

static glm::vec3 bisect(glm::vec3 a, glm::vec3 b)
{
    const float epsilon = 1.0e-8f;

    glm::vec3 v = a + b;
    float length2 = glm::length2(v);
    if (length2 < epsilon) {
        v = glm::vec3(0.0f, 0.0f, 1.0f);
    } else {
        v *= (1.0 / sqrt(length2));
    }

    return v;
}

void ArcballHelper::fill_arc(Mesh & helper, glm::vec3 from, glm::vec3 to, glm::vec3 color)
{
    const int arc_bisects = 5;
    const int arc_segments = 32;

    std::array<glm::vec3, arc_segments + 1> points;
    points[0] = from;
    points[1] = to;
    points[arc_segments] = to;

    // we have our first and last point, we bisect the arc to find the second
    // point
    for (int i = 0; i < arc_bisects; i++) {
        points[1] = bisect(points[0], points[1]);
    }

    const float radius = arcball->radius;

    auto push = [& helper, color, radius](glm::vec3 point) {
        helper.positions.push_back(point * radius);
        if (!helper.use_color) {
            helper.colors.push_back(color);
        } else {
            helper.color = color;
        }
    };

    push(points[0]);
    push(points[1]);

    // using our first two points, we use dynamic programming to build up the
    // remaining points of the arc
    const float dot_two = glm::dot(points[0], points[1]) * 2.0f;
    for (int i = 2; i < arc_segments; i++) {
        points[i] = (points[i - 1] * dot_two) - points[i - 2];
        push(points[i]);
    }

    push(points[arc_segments]);
}

void ArcballHelper::fill_half_arc(Mesh & helper, glm::vec3 axis, glm::vec3 color)
{
    // create a perpendicular vector that is a "mirror" over another axis
    glm::vec3 mirror_point;
    if (axis.z != 1.0f) {
        mirror_point.x = axis.y;
        mirror_point.y = -axis.x;
        mirror_point = glm::normalize(mirror_point);
    } else {
        mirror_point.x = 0.0f;
        mirror_point.y = 1.0f;
    }

    glm::vec3 mid_point = glm::cross(mirror_point, axis);

    // "combine" the two half arcs into one arc
    fill_arc(helper, mirror_point, mid_point, color);
    fill_arc(helper, mid_point, -mirror_point, color);
}

glm::vec3 ArcballHelper::axis_index_color(unsigned int index)
{
    switch (index) {
    case 0:
        return glm::vec3(1.0f, 0.0f, 0.0f);
    case 1:
        return glm::vec3(0.0f, 1.0f, 0.0f);
    case 2:
        return glm::vec3(0.0f, 0.2f, 0.7f);
    default:
        return glm::vec3(1.0f, 1.0f, 1.0f);
    }
}
