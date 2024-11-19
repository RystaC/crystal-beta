#version 450

layout(vertices=3) out;

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

const float min_tess_level = 1.0f;
const float max_tess_level = 32.0f;
const float min_distance = 0.0f;
const float max_distance = 4.0f;

void main() {
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    if(gl_InvocationID == 0) {
        vec4 eye_space0 = view * model * gl_in[0].gl_Position;
        vec4 eye_space1 = view * model * gl_in[1].gl_Position;
        vec4 eye_space2 = view * model * gl_in[2].gl_Position;

        float distance0 = clamp((abs(eye_space0.z) - min_distance) / (max_distance - min_distance), 0.0f, 1.0f);
        float distance1 = clamp((abs(eye_space1.z) - min_distance) / (max_distance - min_distance), 0.0f, 1.0f);
        float distance2 = clamp((abs(eye_space2.z) - min_distance) / (max_distance - min_distance), 0.0f, 1.0f);

        float tess_level0 = mix(max_tess_level, min_tess_level, distance0);
        float tess_level1 = mix(max_tess_level, min_tess_level, distance1);
        float tess_level2 = mix(max_tess_level, min_tess_level, distance2);

        gl_TessLevelOuter[0] = tess_level0;
        gl_TessLevelOuter[1] = tess_level1;
        gl_TessLevelOuter[2] = tess_level2;

        gl_TessLevelInner[0] = max(max(tess_level0, tess_level1), tess_level2);   
    }
}