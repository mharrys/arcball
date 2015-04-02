#version 130

uniform vec3 diffuse = vec3(1.0);
uniform float opacity = 1.0;

out vec4 frag_color;

void main()
{
    frag_color = vec4(diffuse, opacity);
}
