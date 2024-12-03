#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in ivec4 bone_indices;
layout(location = 4) in vec4 bone_weights;
layout(location = 5) in float edge_multiplier;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_tex_coord;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

void main() {
    out_position = vec3(model * vec4(position, 1.0f));
    out_normal = vec3(model * vec4(normal, 0.0f));
    out_tex_coord = tex_coord;
    gl_Position = projection * view * model * vec4(position, 1.0f);
}