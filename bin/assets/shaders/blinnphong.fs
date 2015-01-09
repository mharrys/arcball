#version 130

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float shininess;
};

struct Attenuation {
    float constant;
    float linear;
    float quadratic;
};

struct Light {
    bool enabled;
    vec4 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    Attenuation attenuation;
};

const int MAX_LIGHTS = 2;

uniform float opacity;
uniform Material material;
uniform Light point_lights[MAX_LIGHTS];

in vec4 position;
in vec3 normal;

out vec4 frag_color;

vec3 blinn_phong_model(Light light)
{
    // direction from fragment to light source
    vec3 s = normalize(light.position.xyz - position.xyz);
    // direction vector pointing towards the viewer
    vec3 v = normalize(-position.xyz);
    // halfway vector between the viewer and light source
    vec3 h = normalize(v + s);

    vec3 n = normalize(normal);
    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * material.diffuse * max(dot(s, n), 0.0);
    vec3 specular = max(light.specular * material.specular * pow(max(dot(n, h), 0.0), 4.0 * material.shininess), 0.0);

    return ambient + diffuse + specular;
}

float attenuation(Light light)
{
    float light_distance = distance(light.position.xyz, position.xyz);
    float denom = 0.0;
    denom += light.attenuation.constant;
    denom += light.attenuation.linear * light_distance;
    denom += light.attenuation.quadratic * light_distance * light_distance;
    return 1.0 / denom;
}

void main()
{
    vec3 color = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        Light light = point_lights[i];

        if (!light.enabled) {
            continue;
        }

        color += blinn_phong_model(light) * attenuation(light);
        color += material.emission;
    }

    frag_color = vec4(color, opacity);
}
