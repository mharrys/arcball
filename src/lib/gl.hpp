#ifndef GL_HPP_INCLUDED
#define GL_HPP_INCLUDED

#include "GL/glew.h"

enum VertexAttribIndex {
    VERTEX_POSITION_INDEX = 0,
    VERTEX_NORMAL_INDEX,
    VERTEX_COLOR_INDEX,
    VERTEX_TEX_COORD_INDEX
};

struct GLVertexArray {
    GLuint name;
    GLuint indices_size;
    GLuint positions_size;
    GLuint positions_buffer;
    GLuint normals_buffer;
    GLuint colors_buffer;
    GLuint tex_coords_buffer;
    GLuint indices_buffer;
};

struct GLShader {
    GLenum type;
    GLuint shader;
};

struct GLProgram {
    GLuint program;
};

#endif
