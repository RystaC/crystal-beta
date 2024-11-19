#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec4 color;

layout(location = 0) out vec3 out_normal;

void main() {
    out_normal = normal;
    gl_Position = vec4(tex_coord * 2.0f - 1.0f, 0.0f, 1.0f);
}