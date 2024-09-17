#version 450

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D diffuse_texture;
layout(set = 0, binding = 1) uniform sampler2D specular_texture;

void main() {
    vec3 diffuse = texture(diffuse_texture, in_uv).rgb;
    vec3 specular = texture(specular_texture, in_uv).rgb;

    out_color = vec4(clamp(diffuse + specular, 0.0f, 1.0f), 1.0f);
}