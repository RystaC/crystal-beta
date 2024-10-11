#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec4 color;

layout(location = 0) out vec4 out_color;

const vec3 light_pos = vec3(-5.0f);

void main() {
    // vec3 N = normalize(normal);
    // vec3 L = normalize(light_pos - position);
    // float NdotL = max(dot(N, L), 0.0f);
    // out_color = vec4(vec3(color) * NdotL + vec3(0.1f), 1.0f);
    out_color = color;
}