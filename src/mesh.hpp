#ifndef MESH_HPP_INCLUDED
#define MESH_HPP_INCLUDED

#include "worldobject.hpp"

struct GLVertexArray;

enum class DrawMode {
    POINTS,
    LINE_STRIP,
    LINE_LOOP,
    LINES,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    TRIANGLES
};

enum class DataUsage {
    STATIC,
    DYNAMIC
};

class Mesh : public WorldObject {
public:
    Mesh();
    ~Mesh();

    void draw() override;
    void clear();

    DrawMode mode;
    DataUsage usage;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec2> tex_coords;
    std::vector<glm::uint> indices;

    bool update_positions;
    bool update_normals;
    bool update_colors;
    bool update_tex_coords;
    bool update_indices;

    float opacity;

    glm::vec3 color;
    bool use_color; // overrides vertex colors
private:
    void make();
    void update();
    void buffer_data();
    void define_data();
    void destroy();

    bool needs_update();
    void clear_update();

    std::unique_ptr<GLVertexArray> gl;
};

#endif
