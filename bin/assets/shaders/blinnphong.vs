#version 130

uniform mat4 mv;  // model-view matrix
uniform mat4 mvp; // model-view-projection matrix
uniform mat3 nm;  // normal matrix

in vec4 vertex_position;
in vec3 vertex_normal;

out vec4 position;
out vec3 normal;

void main()
{
    position = mv * vertex_position;
    normal = nm * vertex_normal;
    gl_Position = mvp * vertex_position;
}
