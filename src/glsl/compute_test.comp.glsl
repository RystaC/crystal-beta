#version 450

layout(std430, binding = 0) buffer BufferData {
    float values[];
};
layout(std430, binding = 1) buffer AtomicCounterData {
    uint count;
};

layout(local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

void main() {
    atomicAdd(count, 1);

    uint index = gl_GlobalInvocationID.x;
    if(index >= values.length()) return;

    values[index] = values.length();
}