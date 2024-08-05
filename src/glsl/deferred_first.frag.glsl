#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec4 out_position;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_albedo;

void main() {
    // out_position = position;
    // out_normal = normal;
    // out_albedo = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    out_position = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    out_normal = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    out_albedo = vec4(0.0f, 0.0f, 1.0f, 1.0f);
}