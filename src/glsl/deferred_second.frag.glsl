#version 450

layout(location = 0) out vec4 out_color;

layout(binding = 0, set = 0, input_attachment_index = 0) uniform subpassInput input_position;
layout(binding = 0, set = 0, input_attachment_index = 1) uniform subpassInput input_normal;
layout(binding = 0, set = 0, input_attachment_index = 2) uniform subpassInput input_albedo;

void main() {
    vec3 position = subpassLoad(input_position).rgb;
    vec3 normal = subpassLoad(input_normal).rgb;
    vec3 diffuse_color = subpassLoad(input_albedo).rgb;
    float specular_intensity = subpassLoad(input_albedo).a;

    // out_color = vec4(position, 1.0f);
    // out_color = vec4(normal, 1.0f);
    out_color = vec4(diffuse_color, 1.0f);
    // out_color = vec4(vec3(specular_intensity), 1.0f);
}