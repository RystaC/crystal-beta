#version 450

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0) uniform sampler2D in_texture;

layout(constant_id = 0) const bool is_horizontal = false;

// sigma = 0
// const float[5] weights = float[](
//     0.23437500f,
//     0.19921875f,
//     0.11718750f,
//     0.05078125f,
//     0.01562500f
// );

// sigma = 3
// const float[5] weights = float[](
//     0.15317033f,
//     0.14489292f,
//     0.12264921f,
//     0.09290250f,
//     0.06297020f
// );

// sigma = 5
const float[5] weights = float[](
    0.12611186f,
    0.12361467f,
    0.11641592f,
    0.10533748f,
    0.09157600f
);

ivec2 clamp_coord(ivec2 coord, ivec2 size) {
    return max(min(coord, size - 1), 0);
}

void main() {
    ivec2 coord = ivec2(gl_FragCoord.xy);
    ivec2 size = textureSize(in_texture, 0);

    vec3 sum = weights[0] * texelFetch(in_texture, coord, 0).rgb;
    for(int i = 1; i < 5; ++i) {
        ivec2 offset = is_horizontal ? ivec2(i, 0) : ivec2(0, i);
        sum += weights[i] * texelFetch(in_texture, clamp_coord(coord + offset, size), 0).rgb;
        sum += weights[i] * texelFetch(in_texture, clamp_coord(coord - offset, size), 0).rgb;
    }

    out_color = vec4(sum, 1.0f);
}