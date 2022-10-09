#include <chrono>

#include <iostream>
int main() {
    int startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // set up data
    const int length = 2024;
    float *input = new float[length * length];
    float *output = new float[length * length];
    for (int i = 0; i < length * length; i++) {
        input[i] = (float) rand() / RAND_MAX;
    }
    // matrix multiplication
    for (int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            float sum = 0;
            for (int k = 0; k < length; k++) {
                sum += input[i * length + k] * input[k * length + j];
            }
            output[i * length + j] = sum;
        }
    }
    // Print the time it took to run the compute shader
    int endTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int totalTimeDifferenceInMilliseconds = endTime - startTime;
    std::cout<<"Total time: "<<totalTimeDifferenceInMilliseconds<<std::endl;
    // Print the first 10 elements of the output
    for (int i = 0; i < 10; i++) {
        std::cout << output[i] << std::endl;
    }
    return 0;
}

