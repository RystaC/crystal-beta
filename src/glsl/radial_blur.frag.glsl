#version 450

layout(location = 0) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D in_texture;

layout(constant_id = 0) const uint samples = 16;

const vec2 blur_center = vec2(0.0f);
const float blur_scale = 0.1f;
const float blur_strength = 10.0f;

void main() {
    ivec2 tex_dim = textureSize(in_texture, 0);
    vec2 radial_size = vec2(1.0f / tex_dim.x, 1.0f / tex_dim.y);

    vec2 uv = in_uv;
    vec3 color = vec3(0.0f);
    uv += radial_size * 0.5 - blur_center;

    for(int i = 0; i < samples; ++i) {
        float scale = 1.0f - blur_scale * (float(i) / float(samples - 1));
        color += texture(in_texture, uv * scale + blur_center).rgb;
    }

    color = clamp((color / samples) * blur_strength, 0.0f, 1.0f);

    out_color = vec4(color, 1.0f);
}
