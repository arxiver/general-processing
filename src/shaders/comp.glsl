#version 430 core
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (std430, binding = 0) buffer Input {
    float a[];
} in;
layout (std430, binding = 1) buffer Output {
    float b[];
} out;
void main() {
    uint index = gl_GlobalInvocationID.x;
    output.b[index] = input.a[index] * 2.0 + 1.0;
}