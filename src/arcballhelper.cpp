#include "arcballhelper.hpp"

ArcballHelper::ArcballHelper(
    gst::SurfacePool surfaces,
    std::shared_ptr<gst::RenderState> render_state)
    : show_drag(true),
      show_constraints(true),
      show_result(true),
      show_rim(true),
      override_rim(false)
{
    helpers = gst::Scene(std::make_shared<gst::OrthoCamera>());

    auto create_model_node = [this, &surfaces, &render_state]()
    {
        auto surface = surfaces.create_basic(BASIC_VS, BASIC_FS);

        gst::VertexArray vertex_array(render_state);
        gst::Mesh mesh(vertex_array);
        mesh.positions = {
            render_state,
            { gst::AttribIndex::POSITION, 3, gst::DataType::FLOAT }
        };
        vertex_array.set(mesh.positions);

        auto model = std::make_shared<gst::Model>(mesh, surface);
        auto model_node = std::make_shared<gst::ModelNode>(model);
        helpers.add(model_node);

        return model_node;
    };

    result_node = create_model_node();
    result = result_node->model;
    result->mesh.mode = gst::DrawMode::LINE_STRIP;

    rim_node = create_model_node();
    rim = rim_node->model;
    rim->mesh.mode = gst::DrawMode::LINE_LOOP;
    rim->surface.opacity = 0.4f;
    rim->surface.blend_mode = gst::BlendMode::INTERPOLATIVE;

    drag_node = create_model_node();
    drag = drag_node->model;
    drag->mesh.mode = gst::DrawMode::LINE_STRIP;

    for (int i = 0; i < 3; i++) {
        constraint_nodes[i] = create_model_node();
        constraints[i] = constraint_nodes[i]->model;
        constraints[i]->mesh.mode = gst::DrawMode::LINE_STRIP;
        constraints[i]->surface.blend_mode = gst::BlendMode::INTERPOLATIVE;
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

    helpers.update();
}

void ArcballHelper::draw(gst::Renderer & renderer)
{
    if (!arcball) {
        return;
    }

    drag_node->enabled = show_drag;
    rim_node->enabled = show_rim;
    result_node->enabled = show_result;

    for (auto node : constraint_nodes) {
        node->enabled = show_constraints;
    }

    renderer.render(helpers);
}

void ArcballHelper::update_drag()
{
    drag->mesh.positions.data.clear();

    if (arcball->dragging) {
        auto color = glm::vec3(1.0f, 1.0f, 0.0f);
        // set appropiate drag arc color depending on the constraint axis
        if (arcball->constraint.current != AxisSet::NONE) {
            color = axis_index_color(arcball->constraint.nearest);
        }
        fill_arc(drag->mesh, arcball->drag.from, arcball->drag.to);
        drag->surface.material.diffuse = color;
    }

    drag->mesh.positions.buffer_data(gst::DataUsage::DYNAMIC);
}

void ArcballHelper::update_rim()
{
    rim->mesh.positions.data.clear();

    if (!override_rim) {
        rim->surface.material.diffuse = glm::vec3(0.3f, 0.3f, 0.3f);
        fill_circle(rim->mesh);
    }

    rim->mesh.positions.buffer_data(gst::DataUsage::DYNAMIC);
}

void ArcballHelper::update_constraints()
{
    for (auto constraint : constraints) {
        constraint->mesh.positions.data.clear();
    }

    if (!arcball->allow_constraints) {
        return;
    }

    override_rim = false;
    if (arcball->dragging) {
        // show only focus axis
        if (arcball->constraint.current != AxisSet::NONE) {
            fill_constraint(arcball->constraint.nearest);
            // we use points to "fill" the axis with the drag arc line
            constraints[arcball->constraint.nearest]->mesh.mode = gst::DrawMode::POINTS;
        }
    } else {
        // show all available axes and highlight the nearest axis
        if (arcball->constraint.current != AxisSet::NONE) {
            for (unsigned int i = 0; i < arcball->constraint.available.size(); i++) {
                fill_constraint(i);
            }
        }
    }

    for (auto constraint : constraints) {
        constraint->mesh.positions.buffer_data(gst::DataUsage::DYNAMIC);
    }
}

void ArcballHelper::update_result()
{
    result->mesh.positions.data.clear();
    result->surface.material.diffuse = glm::vec3(1.0f, 0.5f, 0.0f);
    fill_arc(result->mesh, arcball->result.from, arcball->result.to);
}

void ArcballHelper::fill_constraint(unsigned int index)
{
    auto constraint = constraints[index];
    constraint->mesh.mode = gst::DrawMode::LINE_STRIP;
    constraint->surface.material.diffuse = axis_index_color(index);
    constraint->surface.opacity = arcball->constraint.nearest == index ? 1.0f : 0.4f;

    auto axis = arcball->constraint.available[index];
    if (axis.z == 1.0f) {
        // we are looking down through the z-axis
        constraint->mesh.mode = gst::DrawMode::LINE_LOOP;
        fill_circle(constraint->mesh);
        // we signal to not draw our rim to avoid color conflicts when drawing
        override_rim = true;
    } else {
        fill_half_arc(constraint->mesh, axis);
    }
}

void ArcballHelper::fill_circle(gst::Mesh & mesh)
{
    const int segments = 64;
    const float radius = arcball->radius;
    const float PI_2 = PI * 2.0f;
    const float segment = PI_2 / segments;

    for (float i = 0.0f; i < PI_2; i += segment) {
        glm::vec3 position(cos(i), sin(i), 0.0f);
        mesh.positions.data.push_back(position * radius);
    }
}

void ArcballHelper::fill_arc(gst::Mesh & mesh, glm::vec3 from, glm::vec3 to)
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

    const auto radius = arcball->radius;
    auto push = [&mesh, radius](glm::vec3 point) {
        mesh.positions.data.push_back(point * radius);
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

    mesh.positions.buffer_data(gst::DataUsage::DYNAMIC);
}

void ArcballHelper::fill_half_arc(gst::Mesh & mesh, glm::vec3 axis)
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

    auto mid_point = glm::cross(mirror_point, axis);

    // "combine" the two half arcs into one arc
    fill_arc(mesh, mirror_point, mid_point);
    fill_arc(mesh, mid_point, -mirror_point);
}

glm::vec3 ArcballHelper::bisect(glm::vec3 a, glm::vec3 b)
{
    const float epsilon = 1.0e-8f;
    auto v = a + b;
    float length2 = glm::length2(v);
    if (length2 < epsilon) {
        v = glm::vec3(0.0f, 0.0f, 1.0f);
    } else {
        v *= (1.0 / sqrt(length2));
    }
    return v;
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
