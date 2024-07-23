#version 450

layout(location = 0) in vec3 normal;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4((normal + 1.0f) / 2.0f, 1.0f);
    // out_color = color;
}