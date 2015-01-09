#ifndef ARCBALLHELPER_HPP_INCLUDED
#define ARCBALLHELPER_HPP_INCLUDED

#include "arcball.hpp"
#include "assets.hpp"
#include "mesh.hpp"
#include "model.hpp"
#include "modelnode.hpp"
#include "orthocamera.hpp"
#include "renderer.hpp"
#include "renderstate.hpp"
#include "scene.hpp"
#include "surfacepool.hpp"

#include <array>

typedef std::array<std::shared_ptr<gst::Model>, 3> Constraints;
typedef std::array<std::shared_ptr<gst::ModelNode>, 3> ConstraintNodes;

class ArcballHelper {
public:
    ArcballHelper(
        gst::SurfacePool surfaces,
        std::shared_ptr<gst::RenderState> render_state);
    void target(std::shared_ptr<Arcball> arcball);
    void update();
    void draw(gst::Renderer & renderer);

    bool show_drag;
    bool show_constraints;
    bool show_result;
    bool show_rim;
private:
    void update_drag();
    void update_rim();
    void update_constraints();
    void update_result();
    void fill_constraint(unsigned int index);
    void fill_circle(gst::Mesh & mesh);
    void fill_arc(gst::Mesh & mesh, glm::vec3 from, glm::vec3 to);
    void fill_half_arc(gst::Mesh & mesh, glm::vec3 axis);
    glm::vec3 bisect(glm::vec3 a, glm::vec3 b);
    glm::vec3 axis_index_color(unsigned int index);

    std::shared_ptr<Arcball> arcball;

    gst::Scene helpers;
    // draws current dragging arc from two points on the ball
    std::shared_ptr<gst::Model> drag;
    std::shared_ptr<gst::ModelNode> drag_node;
    // draws a circle of the dragging area
    std::shared_ptr<gst::Model> rim;
    std::shared_ptr<gst::ModelNode> rim_node;
    // draws resulting (shortest) arc for reaching the current orientation
    std::shared_ptr<gst::Model> result;
    std::shared_ptr<gst::ModelNode> result_node;
    // draws axis constraints and highlight the nearest axis in the axis set
    // relative to the mouse position
    Constraints constraints;
    ConstraintNodes constraint_nodes;

    bool override_rim;
};

#endif
