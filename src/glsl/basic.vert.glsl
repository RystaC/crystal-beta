#version 450

layout(location = 0) in vec3 position;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
} push_constants;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    gl_Position = push_constants.projection * push_constants.view * push_constants.model * vec4(position, 1.0f);
}