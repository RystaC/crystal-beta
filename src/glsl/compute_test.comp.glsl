#version 450
#extension GL_KHR_shader_subgroup_basic : enable
#extension GL_KHR_shader_subgroup_arithmetic : enable
#extension GL_KHR_shader_subgroup_ballot : enable

layout(std430, binding = 0) buffer BufferData {
    float values[];
};
layout(std430, binding = 1) buffer AtomicCounterData {
    uint count;
};

shared uint global_count;

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
void main() {
    uint index = gl_GlobalInvocationID.x;
    if(index >= values.length()) return;

    uint tmp = subgroupAdd(1);
    if(subgroupElect()) {
        atomicAdd(global_count, tmp);
    }

    values[index] = subgroupExclusiveAdd(1);

    barrier();
    if(index == 0) count = global_count;
}