#ifndef ARCBALLHELPER_HPP_INCLUDED
#define ARCBALLHELPER_HPP_INCLUDED

#include "arcball.hpp"

#include "gust.hpp"

typedef std::array<std::shared_ptr<gst::ModelNode>, 3> ConstraintNodes;

// The responsibility of this class is to show graphical helpers for a
// arcball.
class ArcballHelper {
public:
    // Construct arcball helper with default implementation.
    static ArcballHelper create(gst::ProgramPool & programs);
    // Construct empty arcball helper.
    ArcballHelper() = default;
    // Construct arcball helper. The eye is used to construct the helper
    // scene. The drag node is the dragging arc from two points on the ball.
    // The rim node is the circle of the dragging area. The result node is the
    // resulting (shortest) arc for reaching the current orientation.
    // Constraint nodes are the constraint axes.
    ArcballHelper(
        std::shared_ptr<gst::CameraNode> eye,
        std::shared_ptr<gst::ModelNode> drag_node,
        std::shared_ptr<gst::ModelNode> rim_node,
        std::shared_ptr<gst::ModelNode> result_node,
        ConstraintNodes constraint_nodes);
    // Update helpers to reflect current arcball state.
    void update(Arcball const & arcball);
    // Set visibility of drag arc.
    void set_show_drag(bool show_drag);
    // Set visibility of constraint axes.
    void set_show_constraints(bool show_constraints);
    // Set visibility of result arc.
    void set_show_result(bool show_result);
    // Set visibility of rim.
    void set_show_rim(bool show_rim);
    // Return constructed scene from last update.
    gst::Scene get_helpers() const;
private:
    void update_drag(Arcball const & arcball);
    void update_rim(Arcball const & arcball);
    void update_constraints(Arcball const & arcball);
    void update_result(Arcball const & arcball);

    void fill_constraint(Arcball const & arcball, unsigned int index);
    void fill_circle(Arcball const & arcball, std::vector<glm::vec3> & positions);
    void fill_arc(
        Arcball const & arcball,
        std::vector<glm::vec3> & positions,
        glm::vec3 from,
        glm::vec3 to);
    void fill_half_arc(
        Arcball const & arcball,
        std::vector<glm::vec3> & positions,
        glm::vec3 axis);
    glm::vec3 bisect(glm::vec3 a, glm::vec3 b);
    glm::vec3 axis_index_color(unsigned int index);

    std::shared_ptr<gst::CameraNode> eye;
    gst::Scene helpers;

    std::shared_ptr<gst::ModelNode> drag_node;
    std::shared_ptr<gst::ModelNode> rim_node;
    std::shared_ptr<gst::ModelNode> result_node;
    ConstraintNodes constraint_nodes;

    bool show_drag;
    bool show_constraints;
    bool show_result;
    bool show_rim;
    bool override_rim;
};

#endif
