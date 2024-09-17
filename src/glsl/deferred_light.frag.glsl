#version 450

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0, input_attachment_index = 0) uniform subpassInput input_position;
layout(set = 0, binding = 1, input_attachment_index = 1) uniform subpassInput input_normal;
layout(set = 0, binding = 2, input_attachment_index = 2) uniform subpassInput input_albedo;

struct Light {
    vec3 position;
    vec3 color;
};

#define MAX_LIGHTS 16

layout(set = 1, binding = 0) uniform LightsData {
    vec3 ambient;
    Light lights[MAX_LIGHTS];
} lights_data;

const vec3 camera_pos = vec3(0.0f, 0.0f, -5.0f);

void main() {
    vec3 position = subpassLoad(input_position).rgb;
    vec3 normal = subpassLoad(input_normal).rgb;
    vec3 diffuse_color = subpassLoad(input_albedo).rgb;
    float specular_intensity = subpassLoad(input_albedo).a;

    vec3 N = normalize(normal);
    vec3 V = normalize(camera_pos - position);

    vec3 color = vec3(0.0f);

    // ambient
    color += lights_data.ambient;

    for(int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 L = normalize(lights_data.lights[i].position - position);
        // diffuse
        float NdotL = max(dot(N, L), 0.0f);
        color += lights_data.lights[i].color * NdotL;

        // specilar
        vec3 H = normalize(V + L);
        float NdotH = max(dot(N, H), 0.0f);
        color += vec3(1.0f) * pow(NdotH, 64.0f);
    }

    out_color = vec4(clamp(color, 0.0f, 1.0f), 1.0f);
}