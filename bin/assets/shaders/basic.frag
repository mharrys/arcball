#version 130

uniform float mesh_opacity;

uniform vec3 mesh_color;
uniform bool use_mesh_color;

in vec3 color;

out vec4 frag_color;

void main()
{
    frag_color = vec4(use_mesh_color ? mesh_color : color, mesh_opacity);
}
