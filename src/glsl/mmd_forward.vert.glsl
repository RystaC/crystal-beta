#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coord;
layout(location = 3) in ivec4 bone_indices;
layout(location = 4) in vec4 bone_weights;
layout(location = 5) in float edge_multiplier;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

layout(set = 0, binding = 0) buffer BoneMatrices {
    mat4 bone_matrices[];
};

void main() {
    vec4 final_pos = vec4(0.0f);

    for(int i = 0; i < 4; ++i) {
        if(bone_indices[i] != -1) {
            vec4 local_pos = bone_matrices[bone_indices[i]] * vec4(position, 1.0f);
            final_pos += bone_weights[i] * local_pos;
            vec3 local_normal = vec3(bone_matrices[bone_indices[i]] * vec4(normal, 0.0f));
        }
    }
    gl_Position = projection * view * model * final_pos;
}