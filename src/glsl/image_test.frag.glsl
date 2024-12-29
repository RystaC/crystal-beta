#version 450

layout(location = 0) in vec2 uv;

layout(set = 0, binding = 0) uniform sampler2D texture_sampler;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(texture(texture_sampler, uv).rgb, 1.0f);
    // out_color = vec4(uv, 0.0f, 1.0f);
}