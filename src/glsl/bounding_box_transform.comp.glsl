#version 450

#define FLT_MAX 3.402823466e+38

struct Meshlet {
    vec3 aabb_min;
    uint index_offset;
    vec3 aabb_max;
    uint index_count;
};

struct AABB {
    vec3 box_min;
    vec3 box_max;
};

struct AABBInstance {
    vec3 translate;
    vec3 scale;
};

layout(push_constant) uniform PushConstants {
    mat4 model;
    mat4 view;
    mat4 projection;
};

layout(std430, set = 0, binding = 0) buffer readonly MeshletIn {
    Meshlet meshlets[];
};

layout(std140, set = 0, binding = 1) buffer writeonly AABBOut {
    AABBInstance aabbs_out[];
};

AABB transform_aabb(const in AABB src_box, const in mat4 transform) {
    vec3 box_min = vec3(FLT_MAX);
    vec3 box_max = vec3(-FLT_MAX);

    vec3 vertices[8] = vec3[8](
        vec3(src_box.box_max.x, src_box.box_max.y, src_box.box_max.z),
        vec3(src_box.box_max.x, src_box.box_max.y, src_box.box_min.z),
        vec3(src_box.box_max.x, src_box.box_min.y, src_box.box_max.z),
        vec3(src_box.box_max.x, src_box.box_min.y, src_box.box_min.z),
        vec3(src_box.box_min.x, src_box.box_max.y, src_box.box_max.z),
        vec3(src_box.box_min.x, src_box.box_max.y, src_box.box_min.z),
        vec3(src_box.box_min.x, src_box.box_min.y, src_box.box_max.z),
        vec3(src_box.box_min.x, src_box.box_min.y, src_box.box_min.z)
    );

    for(int i = 0; i < 8; ++i) {
        vec3 vertex = vec3(model * vec4(vertices[i], 1.0f));
        box_min = min(box_min, vertex);
        box_max = max(box_max, vertex);
    }

    return AABB(box_min, box_max);
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
    uint tid = gl_GlobalInvocationID.x;
    if(tid >= meshlets.length()) return;

    AABB box = transform_aabb(AABB(meshlets[tid].aabb_min, meshlets[tid].aabb_max), model);

    vec3 center = abs(box.box_max + box.box_min) / 2.0f;
    vec3 extent = abs(box.box_max - center);

    aabbs_out[tid] = AABBInstance(center, extent);
}