# include "mesh.hpp"

#include "program.hpp"

#include "lib/gl.hpp"

Mesh::Mesh()
    : mode(DrawMode::TRIANGLES),
      usage(DataUsage::STATIC),
      update_positions(false),
      update_normals(false),
      update_colors(false),
      update_tex_coords(false),
      update_indices(false),
      opacity(1.0f),
      use_color(false),
      gl(new GLVertexArray())
{
    make();
}

Mesh::~Mesh()
{
    destroy();
}

static GLenum translate_draw_mode(DrawMode mode)
{
    switch (mode) {
    case DrawMode::POINTS:
        return GL_POINTS;
    case DrawMode::LINE_STRIP:
        return GL_LINE_STRIP;
    case DrawMode::LINE_LOOP:
        return GL_LINE_LOOP;
    case DrawMode::LINES:
        return GL_LINES;
    case DrawMode::TRIANGLE_STRIP:
        return GL_TRIANGLE_STRIP;
    case DrawMode::TRIANGLE_FAN:
        return GL_TRIANGLE_FAN;
    case DrawMode::TRIANGLES:
        return GL_TRIANGLES;
    default:
        return 0;
    }
}

void Mesh::draw()
{
    if (needs_update()) {
        update();
        clear_update();
    }

    GLenum draw_mode = translate_draw_mode(mode);

    glBindVertexArray(gl->name);
    if (gl->indices_size > 0) {
        glDrawElements(draw_mode, gl->indices_size, GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(draw_mode, 0, gl->positions_size);
    }
    glBindVertexArray(0);
}

void Mesh::clear()
{
    positions.clear();
    normals.clear();
    colors.clear();
    tex_coords.clear();
    indices.clear();
}

void Mesh::make()
{
    glGenBuffers(1, &gl->positions_buffer);
    glGenBuffers(1, &gl->normals_buffer);
    glGenBuffers(1, &gl->colors_buffer);
    glGenBuffers(1, &gl->tex_coords_buffer);
    glGenBuffers(1, &gl->indices_buffer);
    glGenVertexArrays(1, &gl->name);

}

static void set_vertex_attrib_state(bool empty, VertexAttribIndex index)
{
    if (empty) {
        glDisableVertexAttribArray(index);
    } else {
        glEnableVertexAttribArray(index);
    }
}

void Mesh::update()
{
    glBindVertexArray(gl->name);

    buffer_data();
    define_data();

    set_vertex_attrib_state(positions.empty(), VERTEX_POSITION_INDEX);
    set_vertex_attrib_state(normals.empty(), VERTEX_NORMAL_INDEX);
    set_vertex_attrib_state(colors.empty(), VERTEX_COLOR_INDEX);
    set_vertex_attrib_state(tex_coords.empty(), VERTEX_TEX_COORD_INDEX);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // store size now in case of client arrays have their size changed without
    // new updates
    gl->indices_size = indices.size();
    gl->positions_size = positions.size();
}

static GLenum translate_data_usage(DataUsage usage)
{
    switch (usage) {
    case DataUsage::STATIC:
        return GL_STATIC_DRAW;
    case DataUsage::DYNAMIC:
        return GL_DYNAMIC_DRAW;
    default:
        return 0;
    }
}

void Mesh::buffer_data()
{
    GLenum data_usage = translate_data_usage(usage);

    if (update_positions) {
        glBindBuffer(GL_ARRAY_BUFFER, gl->positions_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * positions.size(), &positions[0], data_usage);
    }

    if (update_normals) {
        glBindBuffer(GL_ARRAY_BUFFER, gl->normals_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], data_usage);
    }

    if (update_colors) {
        glBindBuffer(GL_ARRAY_BUFFER, gl->colors_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), &colors[0], data_usage);
    }

    if (update_tex_coords) {
        glBindBuffer(GL_ARRAY_BUFFER, gl->tex_coords_buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * tex_coords.size(), &tex_coords[0], data_usage);
    }

    if (update_indices) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->indices_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uint) * indices.size(), &indices[0], data_usage);
    }
}

static void set_vertex_attrib(GLuint buffer, VertexAttribIndex index, GLint size, GLsizei stride, GLsizei start)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(
        index,
        size,
        GL_FLOAT,
        GL_FALSE,
        stride * sizeof(GLfloat),
        (const GLvoid *) (start * sizeof(GLfloat))
    );
}

void Mesh::define_data()
{
    if (!positions.empty()) {
        set_vertex_attrib(gl->positions_buffer, VERTEX_POSITION_INDEX, 3, 0, 0);
    }

    if (!normals.empty()) {
        set_vertex_attrib(gl->normals_buffer, VERTEX_NORMAL_INDEX, 3, 0, 0);
    }

    if (!colors.empty()) {
        set_vertex_attrib(gl->colors_buffer, VERTEX_COLOR_INDEX, 3, 0, 0);
    }

    if (!tex_coords.empty()) {
        set_vertex_attrib(gl->tex_coords_buffer, VERTEX_TEX_COORD_INDEX, 2, 0, 0);
    }

    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->indices_buffer);
    }
}

void Mesh::destroy()
{
    if (gl->positions_buffer > 0) {
        glDeleteBuffers(1, &gl->positions_buffer);
        gl->positions_buffer = 0;
    }

    if (gl->normals_buffer > 0) {
        glDeleteBuffers(1, &gl->normals_buffer);
        gl->normals_buffer = 0;
    }

    if (gl->colors_buffer != 0) {
        glDeleteBuffers(1, &gl->colors_buffer);
        gl->colors_buffer = 0;
    }

    if (gl->tex_coords_buffer != 0) {
        glDeleteBuffers(1, &gl->tex_coords_buffer);
        gl->tex_coords_buffer = 0;
    }

    if (gl->indices_buffer > 0) {
        glDeleteBuffers(1, &gl->indices_buffer);
        gl->indices_buffer = 0;
    }

    if (gl->name > 0) {
        glDeleteVertexArrays(1, &gl->name);
        gl->name = 0;
    }
}

bool Mesh::needs_update()
{
    return update_positions || update_normals || update_colors || update_indices;
}

void Mesh::clear_update()
{
    update_positions = false;
    update_normals = false;
    update_colors = false;
    update_tex_coords = false;
    update_indices = false;
}
