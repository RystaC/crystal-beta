#version 450

layout(location = 0) in vec2 position;

void main() {
    vec2 pos = vec2(position.x * 2.0f - 1.0f, -(position.y * 2.0f - 1.0f));
    gl_Position = vec4(pos, 0.0f, 1.0f);
}