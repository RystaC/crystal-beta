#version 450

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 out_color;

layout(input_attachment_index = 0, binding = 0) uniform subpassInput input_depth;

void main() {
    float depth = subpassLoad(input_depth).x;
    out_color = vec4(vec3(depth), 1.0f);
}