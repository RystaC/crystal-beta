#version 450

layout(isolines) in;

void main() {
    float t = gl_TessCoord.x;

    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;

    float b0 = (1.0f - t) * (1.0f - t);
    float b1 = 2.0f * (1.0f - t) * t;
    float b2 = t * t;

    vec4 pos = b0 * p0 + b1 * p1 + b2 * p2;
    gl_Position = pos;
}