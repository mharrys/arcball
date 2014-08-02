#version 130

// material ambient and diffuse
uniform vec3 Ka = vec3(0.2);
uniform vec3 Kd = vec3(0.5);

// light source ambient and diffuse
uniform vec3 La = vec3(0.2);
uniform vec3 Ld = vec3(0.5);

uniform vec4 light_position;

in vec3 position;
in vec3 normal;

out vec4 frag_color;

vec3 lambert_model()
{
    vec3 s;

    if (light_position.w == 0) {
        s = normalize(light_position.xyz);
    } else {
        s = normalize(light_position.xyz - position.xyz);
    }

    vec3 ambient = La * Ka;
    vec3 diffuse = Ld * Kd * max(dot(s, normal), 0.0);

    return ambient + diffuse;
}

void main()
{
    frag_color = vec4(lambert_model(), 1.0);
}
