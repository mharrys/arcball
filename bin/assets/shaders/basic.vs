#version 130

uniform mat4 model_view;
uniform mat4 projection;

in vec4 vertex_position;

void main()
{
    gl_Position = projection * model_view * vertex_position;
}
