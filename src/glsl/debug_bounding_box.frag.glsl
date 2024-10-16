#version 450

layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(vec3(1.0f), 0.5f);
}