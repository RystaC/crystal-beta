#version 450

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D in_texture;

layout(constant_id = 0) const bool is_horizontal = false;

const float[5] weights = float[](
    0.2270270f,
    0.1945945f,
    0.1216216f,
    0.0540540f,
    0.0162162f
);

ivec2 clamp_coord(ivec2 coord, ivec2 size) {
    return max(min(coord, size - 1), 0);
}

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    ivec2 size = textureSize(in_texture, 0);

    vec3 sum = weights[0] * texelFetch(in_texture, coord, 0).rgb;
    for(int i = 1; i < 5; ++i) {
        ivec2 offset = params.is_horizontal ? ivec2(i, 0) : ivec2(0, i);
        sum += weights[i] * texelFetch(in_texture, clamp_coord(coord + offset, size), 0).rgb;
        sum += weights[i] * texelFetch(in_texture, clamp_coord(coord - offset, size), 0).rgb;
    }

    out_color = vec4(sum, 1.0f);
}