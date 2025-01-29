#version 330 core

out vec4 FragColor;

uniform vec3 view_position;

in vec3 frag_normal_vec;
in vec3 frag_frag_pos;
in vec2 frag_text_coords;

struct Material {
    sampler2D diffuse;
    sampler2D specular; 
    float shininess;
};

// Light types
#define LT_POINT       0
#define LT_DIRECTIONAL 1
#define LT_SPOT        2

struct Light {
    int type;

    vec3 vec;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    vec3 direction;
    float cutoff;
    float outer_cutoff;
};

struct ADS {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
uniform Material material;

ADS ambient_diffuse_specular(Light l, Material m, vec3 light_direction) {
    vec3 view_direction = normalize(frag_frag_pos - view_position);

    vec3 reflect_direction = reflect(light_direction, frag_normal_vec);

    float diffuse_scalar = max(dot(frag_normal_vec, light_direction), 0.0);
    vec3 diffuse_color = l.diffuse * diffuse_scalar * texture(m.diffuse, frag_text_coords).rgb;

    float spec = pow(max(dot(view_direction, reflect_direction), 0.0), m.shininess);
    vec3 specular = l.specular * spec * texture(m.specular, frag_text_coords).rgb;

    vec3 ambient_color = l.ambient * texture(m.diffuse, frag_text_coords).rgb;

    ADS final_light = ADS(ambient_color, diffuse_color, specular);

    return final_light;
}

float calc_attenuation(Light l, vec3 light_direction) {
    float light_distance = length(l.vec - frag_frag_pos);
    float attenuation = 1.0 / (l.constant + l.linear * light_distance + l.quadratic * (light_distance * light_distance));

    return attenuation;
}

ADS spot_light(Light l, Material m) {
    ADS final_light;

    // Points from the fragment to the light source
    vec3 frag_direction = normalize(l.vec - frag_frag_pos);
    // Angle between frag_direction and l.direction
    float theta = dot(normalize(-l.direction), frag_direction);

    if (theta > l.outer_cutoff) {
        float intensity = (theta - l.outer_cutoff) / (l.cutoff - l.outer_cutoff);
        intensity = clamp(intensity, 0.02, 1.0);

        final_light = ambient_diffuse_specular(l, m, frag_direction);
        float attenuation = calc_attenuation(l, frag_direction);

        final_light.ambient *= attenuation;
        final_light.diffuse *= intensity * attenuation;
        final_light.specular *= intensity * attenuation;

    } else {
        final_light.ambient = l.ambient * texture(m.diffuse, frag_text_coords).rgb;
        final_light.diffuse = vec3(0.0);
        final_light.specular = vec3(0.0);
    }

    return final_light;
}

ADS point_light(Light l, Material m) {
    ADS final_light;

    vec3 light_direction = normalize(l.vec - frag_frag_pos);

    float attenuation = calc_attenuation(l, light_direction);

    final_light = ambient_diffuse_specular(l, m, light_direction);
    final_light.ambient *= attenuation;
    final_light.diffuse *= attenuation;
    final_light.specular *= attenuation;
    return  final_light;
}

ADS directional_light(Light l, Material m) {
    return ambient_diffuse_specular(l, m, l.vec);
}

void main() {
    ADS final_light;

    if (light.type == LT_POINT) {
        final_light = point_light(light, material);
    }

    else if (light.type == LT_DIRECTIONAL) {
        final_light = directional_light(light, material);
    }

    else if (light.type == LT_SPOT) {
        final_light = spot_light(light, material);
    }

    FragColor = vec4(final_light.ambient + final_light.diffuse + final_light.specular, 1.0);
}