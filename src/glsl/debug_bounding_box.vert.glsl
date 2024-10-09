#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec4 color;

layout(location = 4) in vec3 translate_instance;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

out gl_PerVertex { vec4 gl_Position; };

void main() {
    mat4 mat_instance = mat4(vec4(1.0f, 0.0f, 0.0f, 0.0f), vec4(0.0f, 1.0f, 0.0f, 0.0f), vec4(0.0f, 0.0f, 1.0f, 0.0f), vec4(translate_instance, 1.0f));
    vec3 right = vec3(1.0f, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 forward = vec3(0.0f, 0.0f, 1.0f);
    vec3 new_right = (model * vec4(right, 0.0f)).xyz;
    vec3 new_up = (model * vec4(up, 0.0f)).xyz;
    vec3 new_forward = (model * vec4(forward, 0.0f)).xyz;
    float scale_x = abs(dot(right, new_right)) + abs(dot(right, new_up)) + abs(dot(right, new_forward));
    float scale_y = abs(dot(up, new_right)) + abs(dot(up, new_up)) + abs(dot(up, new_forward));
    float scale_z = abs(dot(forward, new_right)) + abs(dot(forward, new_up)) + abs(dot(forward, new_forward));
    mat4 new_scale = mat4(vec4(scale_x, 0.0f, 0.0f, 0.0f), vec4(0.0f, scale_y, 0.0f, 0.0f), vec4(0.0f, 0.0f, scale_z, 0.0f), vec4(0.0f, 0.0f, 0.0f, 1.0f));
    gl_Position = projection * view * mat_instance * new_scale * vec4(position, 1.0f);
}