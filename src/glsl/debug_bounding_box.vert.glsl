#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in vec4 color;

layout(location = 4) in vec3 instance_translate;
layout(location = 5) in vec3 instance_scale;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

out gl_PerVertex { vec4 gl_Position; };

void main() {
    mat4 mat_instance = mat4(vec4(instance_scale.x, 0.0f, 0.0f, 0.0f), vec4(0.0f, instance_scale.y, 0.0f, 0.0f), vec4(0.0f, 0.0f, instance_scale.z, 0.0f), vec4(instance_translate, 1.0f));

    gl_Position = projection * view * mat_instance * vec4(position, 1.0f);
}