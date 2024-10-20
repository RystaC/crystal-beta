#version 450

struct Meshlet {
    vec3 aabb_min;
    uint index_offset;
    vec3 aabb_max;
    uint index_count;
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
};

layout(std430, set = 0, binding = 0) buffer readonly MeshletIn {
    Meshlet meshlets[];
};

layout(std430, set = 0, binding = 1) buffer readonly IndicesIn {
    uint indices_in[];
};

layout(std430, set = 0, binding = 2) buffer writeonly IndicesOut {
    uint indices_out[];
};

layout(std430, set = 0, binding = 3) buffer writeonly IndirectBuffer {
    DrawIndexedIndirectBuffer indirect_buffer;
};

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
    uint tid = gl_GlobalInvocationID.x;
    if(tid >= meshlets.length()) return;
    if(tid == 0) {
        indirect_buffer = DrawIndexedIndirectBuffer(0, 1, 0, 0, 0);
    }
}