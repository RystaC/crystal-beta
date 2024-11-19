#version 450

layout(location = 0) in vec3 normal;

layout(location = 0) out vec4 out_color;

void main() {
    vec3 N = normalize(normal) * 0.5f + 0.5f;
    out_color = vec4(N, 1.0f);
}