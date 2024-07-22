#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec4 out_color;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
} push_constants;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    out_normal = normal;
    out_color = color;
    gl_Position = push_constants.projection * push_constants.view * push_constants.model * vec4(position, 1.0f);
}