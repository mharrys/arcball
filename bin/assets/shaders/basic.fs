#version 130

struct Material {
    vec3 diffuse;
};

uniform float opacity;
uniform Material material;

out vec4 frag_color;

void main()
{
    frag_color = vec4(material.diffuse, opacity);
}
