#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform UniformBufferData {
    vec3 light_position;
} uniforms;

void main() {
    // out_color = vec4((normal + 1.0f) / 2.0f, 1.0f);
    // out_color = color;
    vec3 L = normalize(uniforms.light_position - position);
    vec3 N = normalize(normal);
    float NdotL = dot(N, L);
    out_color = vec4(vec3(1.0f) * max(NdotL, 0.0f), 1.0f);
}