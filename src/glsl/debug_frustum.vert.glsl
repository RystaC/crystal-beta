#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(push_constant) uniform PushConstants {
    mat4 view_proj;
    mat4 inv_view_proj;
};

out gl_PerVertex { vec4 gl_Position; };

void main() {
    // gl_Position = inv_view_proj * vec4(position, 1.0f);
    gl_Position = view_proj * inv_view_proj * vec4(position, 1.0f);
}