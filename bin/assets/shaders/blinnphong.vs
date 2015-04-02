#version 130

uniform mat4 model_view;
uniform mat4 projection;
uniform mat3 nm; // normal matrix

in vec4 vertex_position;
in vec3 vertex_normal;

out vec4 position;
out vec3 normal;

void main()
{
    position = model_view * vertex_position;
    normal = nm * vertex_normal;
    gl_Position = projection * model_view * vertex_position;
}
