#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;

layout(location = 0) out vec4 out_color;

void main() {
    vec2 pos = vec2(position.x * 2.0f - 1.0f, -(position.y * 2.0f - 1.0f));
    out_color = color;
    gl_PointSize = 5.0f;
    gl_Position = vec4(pos, 0.0f, 1.0f);
}