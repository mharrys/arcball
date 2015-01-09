#version 130

uniform mat4 mvp; // model-view-projection matrix

in vec3 vertex_position;

void main()
{
    gl_Position = mvp * vec4(vertex_position, 1.0);
}
