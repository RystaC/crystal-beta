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

layout(local_size_x = 128, local_size_y = 1, local_size_z = 1) in;
void main() {
    atomicAdd(count, 1);

    uint index = gl_GlobalInvocationID.x;
    if(index >= values.length()) return;

    uint tmp = subgroupAdd(1);
    if(subgroupElect()) {
        atomicAdd(count, tmp);
    }

    values[index] = subgroupExclusiveAdd(1);
}