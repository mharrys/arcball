#include "arcballhelper.hpp"

#include "assets.hpp"

ArcballHelper ArcballHelper::create(gst::ProgramPool & programs)
{
    auto camera = std::unique_ptr<gst::Camera>(new gst::OrthoCamera());
    auto eye = std::make_shared<gst::CameraNode>(std::move(camera));

    auto create_model_node = [&programs]()
    {
        auto basic_program = programs.create(BASIC_VS, BASIC_FS);
        auto basic_pass = std::make_shared<gst::BasicPass>(basic_program);

        auto vertex_array = std::make_shared<gst::VertexArrayImpl>();
        auto mesh = gst::Mesh(vertex_array);
        auto material = gst::Material::create_free();
        auto model = gst::Model(mesh, material, basic_pass);

        return std::make_shared<gst::ModelNode>(model);
    };

    auto result_node = create_model_node();
    result_node->get_mesh().set_draw_mode(gst::DrawMode::LINE_STRIP);

    auto rim_node = create_model_node();
    rim_node->get_mesh().set_draw_mode(gst::DrawMode::LINE_LOOP);
    rim_node->get_material().get_uniform("opacity") = 0.4f;
    rim_node->get_pass().set_blend_mode(gst::BlendMode::INTERPOLATIVE);

    auto drag_node = create_model_node();
    drag_node->get_mesh().set_draw_mode(gst::DrawMode::LINE_STRIP);

    ConstraintNodes constraint_nodes;
    for (int i = 0; i < 3; i++) {
        constraint_nodes[i] = create_model_node();
        constraint_nodes[i]->get_mesh().set_draw_mode(gst::DrawMode::LINE_STRIP);
        constraint_nodes[i]->get_pass().set_blend_mode(gst::BlendMode::INTERPOLATIVE);
    }

    return ArcballHelper(eye, drag_node, rim_node, result_node, constraint_nodes);
}

ArcballHelper::ArcballHelper(
    std::shared_ptr<gst::CameraNode> eye,
    std::shared_ptr<gst::ModelNode> drag_node,
    std::shared_ptr<gst::ModelNode> rim_node,
    std::shared_ptr<gst::ModelNode> result_node,
    ConstraintNodes constraint_nodes)
    : eye(eye),
      drag_node(drag_node),
      rim_node(rim_node),
      result_node(result_node),
      constraint_nodes(constraint_nodes),
      show_drag(true),
      show_constraints(true),
      show_result(true),
      show_rim(true),
      override_rim(false)
{
}

void ArcballHelper::update(Arcball const & arcball)
{
    update_drag(arcball);
    update_constraints(arcball);
    update_result(arcball);
    update_rim(arcball);

    helpers = gst::Scene(eye);

    if (show_drag) {
        helpers.add(drag_node);
    }

    if (show_rim) {
        helpers.add(rim_node);
    }

    if (show_result) {
        helpers.add(result_node);
    }

    if (show_constraints) {
        for (auto node : constraint_nodes) {
            helpers.add(node);
        }
    }

    helpers.update();
}

void ArcballHelper::set_show_drag(bool show_drag)
{
    this->show_drag = show_drag;
}

void ArcballHelper::set_show_constraints(bool show_constraints)
{
    this->show_constraints = show_constraints;
}

void ArcballHelper::set_show_result(bool show_result)
{
    this->show_result = show_result;
}

void ArcballHelper::set_show_rim(bool show_rim)
{
    this->show_rim = show_rim;
}

gst::Scene ArcballHelper::get_helpers() const
{
    return helpers;
}

void ArcballHelper::update_drag(Arcball const & arcball)
{
    std::vector<glm::vec3> positions;

    if (arcball.dragging) {
        auto color = glm::vec3(1.0f, 1.0f, 0.0f);
        // set appropiate drag arc color depending on the constraint axis
        if (arcball.constraint.current != AxisSet::NONE) {
            color = axis_index_color(arcball.constraint.nearest);
        }
        auto & material = drag_node->get_material();
        material.get_uniform("diffuse") = color;

        fill_arc(arcball, positions, arcball.drag.from, arcball.drag.to);
    }

    auto & mesh = drag_node->get_mesh();
    mesh.set_positions(positions);
}

void ArcballHelper::update_rim(Arcball const & arcball)
{
    std::vector<glm::vec3> positions;

    if (!override_rim) {
        auto & material = rim_node->get_material();
        material.get_uniform("diffuse") = glm::vec3(0.3f, 0.3f, 0.3f);
        fill_circle(arcball, positions);
    }

    auto & mesh = rim_node->get_mesh();
    mesh.set_positions(positions);
}

void ArcballHelper::update_constraints(Arcball const & arcball)
{
    for (auto node : constraint_nodes) {
        node->get_mesh().set_positions({});
    }

    if (!arcball.allow_constraints) {
        return;
    }

    override_rim = false;
    if (arcball.dragging) {
        // show only focus axis
        if (arcball.constraint.current != AxisSet::NONE) {
            fill_constraint(arcball, arcball.constraint.nearest);
            // we use points to "fill" the axis with the drag arc line
            auto & mesh = constraint_nodes[arcball.constraint.nearest]->get_mesh();
            mesh.set_draw_mode(gst::DrawMode::POINTS);
        }
    } else {
        // show all available axes and highlight the nearest axis
        if (arcball.constraint.current != AxisSet::NONE) {
            for (unsigned int i = 0; i < arcball.constraint.available.size(); i++) {
                fill_constraint(arcball, i);
            }
        }
    }
}

void ArcballHelper::update_result(Arcball const & arcball)
{
    std::vector<glm::vec3> positions;
    fill_arc(arcball, positions, arcball.result.from, arcball.result.to);

    auto & material = result_node->get_material();
    material.get_uniform("diffuse") = glm::vec3(1.0f, 0.5f, 0.0f);

    auto & mesh = result_node->get_mesh();
    mesh.set_positions(positions);
}

void ArcballHelper::fill_constraint(Arcball const & arcball, unsigned int index)
{
    std::vector<glm::vec3> positions;
    auto & mesh = constraint_nodes[index]->get_mesh();
    auto & material = constraint_nodes[index]->get_material();

    mesh.set_draw_mode(gst::DrawMode::LINE_STRIP);
    material.get_uniform("diffuse") = axis_index_color(index);
    material.get_uniform("opacity") = arcball.constraint.nearest == index ? 1.0f : 0.4f;

    auto axis = arcball.constraint.available[index];
    if (axis.z == 1.0f) {
        // we are looking down through the z-axis
        mesh.set_draw_mode(gst::DrawMode::LINE_LOOP);
        fill_circle(arcball, positions);
        // we signal to not draw our rim to avoid color conflicts when drawing
        override_rim = true;
    } else {
        fill_half_arc(arcball, positions, axis);
    }

    mesh.set_positions(positions);
}

void ArcballHelper::fill_circle(Arcball const & arcball, std::vector<glm::vec3> & positions)
{
    const auto segments = 64;
    const auto radius = arcball.radius;
    const auto PI_2 = PI * 2.0f;
    const auto segment = PI_2 / segments;

    for (auto i = 0.0f; i < PI_2; i += segment) {
        glm::vec3 position(cos(i), sin(i), 0.0f);
        positions.push_back(position * radius);
    }
}

void ArcballHelper::fill_arc(
    Arcball const & arcball,
    std::vector<glm::vec3> & positions,
    glm::vec3 from,
    glm::vec3 to)
{
    const auto arc_bisects = 5;
    const auto arc_segments = 32;

    std::array<glm::vec3, arc_segments + 1> points;
    points[0] = from;
    points[1] = to;
    points[arc_segments] = to;

    // we have our first and last point, we bisect the arc to find the second
    // point
    for (int i = 0; i < arc_bisects; i++) {
        points[1] = bisect(points[0], points[1]);
    }

    const auto radius = arcball.radius;
    auto push = [&positions, radius](glm::vec3 point) {
        positions.push_back(point * radius);
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

void ArcballHelper::fill_half_arc(
    Arcball const & arcball,
    std::vector<glm::vec3> & positions,
    glm::vec3 axis)
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
    fill_arc(arcball, positions, mirror_point, mid_point);
    fill_arc(arcball, positions, mid_point, -mirror_point);
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
