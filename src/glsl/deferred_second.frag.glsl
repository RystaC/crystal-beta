#version 450

layout(location = 0) out vec4 out_color;

layout(set = 0, binding = 0, input_attachment_index = 0) uniform subpassInput input_position;
layout(set = 0, binding = 1, input_attachment_index = 1) uniform subpassInput input_normal;
layout(set = 0, binding = 2, input_attachment_index = 2) uniform subpassInput input_albedo;

const vec3 light_pos = vec3(5.0f, 5.0f, -5.0f);
const vec3 camera_pos = vec3(0.0f, 0.0f, -5.0f);

void main() {
    vec3 position = subpassLoad(input_position).rgb;
    vec3 normal = subpassLoad(input_normal).rgb;
    vec3 diffuse_color = subpassLoad(input_albedo).rgb;
    float specular_intensity = subpassLoad(input_albedo).a;

    vec3 N = normalize(normal);
    vec3 V = normalize(camera_pos - position);
    vec3 L = normalize(light_pos - position);

    vec3 color = vec3(0.0f);

    // ambient
    color += diffuse_color * vec3(0.1f);

    // diffuse
    float NdotL = max(dot(N, L), 0.0f);
    color += diffuse_color * NdotL;

    // specilar
    vec3 H = normalize(V + L);
    float NdotH = max(dot(N, H), 0.0f);
    color += vec3(1.0f) * pow(NdotH, 64.0f);

    out_color = vec4(clamp(color, 0.0f, 1.0f), 1.0f);
}