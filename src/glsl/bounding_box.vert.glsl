#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 3) in vec3 translate_instance;

layout(location = 4) in vec3 scale_instance;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
} push_constants;

out gl_PerVertex { vec4 gl_Position; };

void main() {
    mat4 mat_instance = mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(translate_instance, 1.0f));
    mat4 scale = mat4(vec4(scale_instance.x, 0.0f, 0.0f, 0.0f), vec4(0.0f, scale_instance.y, 0.0f, 0.0f), vec4(0.0f, 0.0f, scale_instance.z, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
    gl_Position = push_constants.projection * push_constants.view * mat_instance * push_constants.model * scale * vec4(position, 1.0f);
}