#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 out_color;

#define MAX_LIGHTS 3

struct Light {
    vec3 position;
    vec3 color;
};

layout(binding = 0) uniform UniformBufferData {
    vec3 camera_pos;
    vec3 ambient;
    Light lights[MAX_LIGHTS];
} uniforms;

void main() {
    vec3 N = normalize(normal);
    vec3 V = normalize(uniforms.camera_pos - position);
    out_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    // ambient term
    vec3 ambient = uniforms.ambient;
    out_color += vec4(ambient, 1.0f);

    // diffuse term
    for(int i = 0; i < MAX_LIGHTS; ++i) {
        vec3 L = normalize(uniforms.lights[i].position - position);
        float NdotL = max(dot(N, L), 0.0f);
        vec3 diffuse = uniforms.lights[i].color * (N + 1.0f) / 2.0f * NdotL;
        out_color += vec4(diffuse, 1.0f);

        // specular term
        vec3 H = normalize(V + L);
        float NdotH = max(dot(N, H), 0.0f);
        vec3 specular = uniforms.lights[i].color * vec3(1.0f) * pow(NdotH, 64.0f);
        out_color += vec4(specular, 1.0f);
    }

    out_color = clamp(out_color, 0.0f, 1.0f);
}