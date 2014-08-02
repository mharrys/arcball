#version 130

uniform mat4 mv;  // model-view matrix
uniform mat4 mvp; // model-view-projection matrix
uniform mat3 nm;  // normal matrix

in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 position;
out vec3 normal;

void main()
{
    position = (mv * vec4(vertex_position, 1.0)).xyz;
    normal = normalize(nm * vertex_normal);

    gl_Position = mvp * vec4(vertex_position, 1.0);
}
