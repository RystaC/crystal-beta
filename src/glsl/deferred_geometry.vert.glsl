#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 3) in vec3 translate_instance;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec4 out_color;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

out gl_PerVertex { vec4 gl_Position; };

void main() {
    mat4 mat_instance = mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(translate_instance, 1.0f));
    // mat4 mat_instance = mat4(1.0f);
    out_position = vec3(mat_instance * model * vec4(position, 1.0f));
    out_normal = vec3(mat_instance * model * vec4(normal, 0.0f));
    out_color = color;
    gl_Position = projection * view * mat_instance * model * vec4(position, 1.0f);
}