#version 450

layout(location = 0) in vec3 instance_position;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    gl_PointSize = 8.0f;
    gl_Position = projection * view * model * vec4(instance_position, 1.0f);
}