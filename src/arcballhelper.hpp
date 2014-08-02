#ifndef ARCBALLHELPER_HPP_INCLUDED
#define ARCBALLHELPER_HPP_INCLUDED

#include "arcball.hpp"
#include "mesh.hpp"
#include "camera.hpp"
#include "program.hpp"

#include <array>
#include <memory>

class ArcballHelper {
public:
    ArcballHelper();

    void target(std::shared_ptr<Arcball> arcball);
    void update();
    void draw(Program & program, Camera & camera);

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
    void fill_circle(Mesh & helper, glm::vec3 color);
    void fill_arc(Mesh & helper, glm::vec3 from, glm::vec3 to, glm::vec3 color);
    void fill_half_arc(Mesh & helper, glm::vec3 axis, glm::vec3 color);

    glm::vec3 axis_index_color(unsigned int index);

    std::shared_ptr<Arcball> arcball;

    Mesh drag;
    Mesh rim;
    Mesh result;
    std::array<Mesh, 3> constraints;

    bool override_rim;
};

#endif
