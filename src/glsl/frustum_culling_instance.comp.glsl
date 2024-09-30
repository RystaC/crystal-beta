#version 450

struct AABB {
    vec3 min;
    vec3 max;
};

struct InstanceData {
    vec3 translate;
};

struct DrawIndexedIndirectBuffer {
    uint index_count;
    uint instance_count;
    uint first_index;
    int vertex_offset;
    uint first_instance;
};

layout(push_constant) uniform ExternalData {
    // view frustum
    vec4 planes[6];
    // global transformation
    mat4 model;
    // model space bounding box
    AABB box;
};

layout(std430, set = 0, binding = 0) buffer readonly InstanceIn {
    InstanceData instance_in[];
};

layout(std430, set = 0, binding = 1) buffer writeonly InstanceOut {
    InstanceData instance_out[];
};

layout(std430, set = 0, binding = 2) buffer writeonly IndirectBuffer {
    DrawIndexedIndirectBuffer indirect_buffer;
};

AABB transform_aabb(const in AABB src_box, const in mat4 transform) {
    vec3 center = (src_box.min + src_box.max) / 2.0f;

    vec3 right = vec3(src_box.max.x - center.x, 0.0f, 0.0f);
    vec3 up = vec3(0.0f, src_box.min.y - center.y, 0.0f);
    vec3 forward = vec3(0.0f, 0.0f, src_box.max.z - center.z);

    vec3 new_center = center + vec3(transform * vec4(0.0f, 0.0f, 0.0f, 1.0f));

    vec3 new_right = vec3(transform * vec4(right, 0.0f));
    vec3 new_up = vec3(transform * vec4(up, 0.0f));
    vec3 new_forward = vec3(transform * vec4(forward, 0.0f));

    float scale_x = abs(dot(right, new_right)) + abs(dot(right, new_up)) + abs(dot(right, new_forward));
    float scale_y = abs(dot(up, new_right)) + abs(dot(up, new_up)) + abs(dot(up, new_forward));
    float scale_z = abs(dot(forward, new_right)) + abs(dot(forward, new_up)) + abs(dot(forward, new_forward));

    vec3 new_box_min = vec3(new_center.x - right.x * scale_x, new_center.y + up.y * scale_y, new_center.z - forward.z * scale_z);
    vec3 new_box_max = vec3(new_center.x + right.x * scale_x, new_center.y - up.y * scale_y, new_center.z + forward.z * scale_z);

    return AABB(new_box_min, new_box_max);
}

bool is_in_frustum(const in AABB box) {
    for(int i = 0; i < 6; ++i) {
        int r = 0;
        r += (dot(planes[i], vec4(box.min.x, box.min.y, box.min.z, 1.0f)) < 0.0f) ? 1 : 0;
        r += (dot(planes[i], vec4(box.max.x, box.min.y, box.min.z, 1.0f)) < 0.0f) ? 1 : 0;
        r += (dot(planes[i], vec4(box.min.x, box.max.y, box.min.z, 1.0f)) < 0.0f) ? 1 : 0;
        r += (dot(planes[i], vec4(box.max.x, box.max.y, box.min.z, 1.0f)) < 0.0f) ? 1 : 0;
        r += (dot(planes[i], vec4(box.min.x, box.min.y, box.max.z, 1.0f)) < 0.0f) ? 1 : 0;
        r += (dot(planes[i], vec4(box.max.x, box.min.y, box.max.z, 1.0f)) < 0.0f) ? 1 : 0;
        r += (dot(planes[i], vec4(box.min.x, box.max.y, box.max.z, 1.0f)) < 0.0f) ? 1 : 0;
        r += (dot(planes[i], vec4(box.max.x, box.max.y, box.max.z, 1.0f)) < 0.0f) ? 1 : 0;
        if(r == 8) return false;
    }

    return true;
}

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
    uint tid = gl_GlobalInvocationID.x;
    if(tid >= instance_in.length()) return;
    if(tid == 0) {
        // reset indirect buffer (keep index count)
        indirect_buffer = DrawIndexedIndirectBuffer(indirect_buffer.index_count, 0, 0, 0, 0);
    }

    barrier();

    mat4 instance_transform = mat4(
        vec4(1.0f, 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 1.0f, 0.0f, 0.0f),
        vec4(0.0f, 0.0f, 1.0f, 0.0f),
        vec4(instance_in[tid].translate, 1.0f)
    );

    mat4 transform = instance_transform * model;

    AABB instance_box = transform_aabb(box, transform);
    
    if(is_in_frustum(instance_box)) {
        uint index = atomicAdd(indirect_buffer.instance_count, 1);
        instance_out[index] = instance_in[tid];
    }
}