#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>

#include <iostream>
// Make a compute shader for a simple multiply-add operation of two vectors
static const char* GL_SHADER_SRC = R"(
    #version 430
    #extension GL_ARB_compute_shader : require
    #extension GL_ARB_explicit_uniform_location : require
    #extension GL_ARB_shader_storage_buffer_object : require

    layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

    layout(std430, binding = 0) buffer ssbOutput {
        float gOutput[];
    };
    layout(std430, binding = 1) buffer ssbInput {
        float gInput[];
    };
    layout(location = 2) uniform uint uLength;

    void main() {
        uint destColumn = gl_GlobalInvocationID[0], destRow = gl_GlobalInvocationID[1];
        float value = 0.0f;
        for (uint i = 0u; i < uLength; i++)
            value += gInput[i * uLength + destColumn] * gInput[destRow * uLength + i];
        gOutput[destRow * uLength + destColumn] = value;
    }
)";

static const char* CL_SHADER_SRC = R"(
    __kernel void matrix_multiply(__global __write_only float *gOutput,
                                  __global __read_only float *gInput,
                                  uint kLength) {
        uint destColumn = get_global_id(0), destRow = get_global_id(1);
        float value = 0.0f;
        for (uint i = 0; i < kLength; i++)
            value += gInput[i * kLength + destColumn] * gInput[destRow * kLength + i];
        gOutput[destRow * kLength + destColumn] = value;
    }
)";


int main() {
    // Init time state 
    int startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(320, 240, "Compute Shader", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Load the shader program
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(shader, 1, &GL_SHADER_SRC, nullptr);
    glCompileShader(shader);

    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED" << std::endl << infoLog << std::endl;
    }
    
    // Create the shader program
    GLuint prog = glCreateProgram();
    glAttachShader(prog, shader);
    glLinkProgram(prog);
    // Check for linking errors
    glGetProgramiv(prog, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(prog, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl << infoLog << std::endl;
    }
    // set up the matrix data
    const int length = 10024;
    float *input = new float[length * length];
    float *output = new float[length * length];
    for (int i = 0; i < length * length; i++) {
        input[i] = (float) rand() / RAND_MAX;
    }

    // Create the input buffer
    GLuint inputBuffer;
    glGenBuffers(1, &inputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, length * length * sizeof(float), input, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, inputBuffer);

    // Create the output buffer
    GLuint outputBuffer;
    glGenBuffers(1, &outputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, length * length * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, outputBuffer);

    // Set the uniform
    glUseProgram(prog);
    GLuint lengthLoc = glGetUniformLocation(prog, "uLength");
    glUniform1ui(lengthLoc, length);

    // Run the compute shader
    glDispatchCompute(length / 32, length / 32, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Copy the output buffer to the host
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, length * length * sizeof(float), output);

    // Print the time it took to run the compute shader
    int endTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "Time: " << endTime - startTime << "ms" << std::endl;

    // Print the first 10 elements of the output
    for (int i = 0; i < 10; i++) {
        std::cout << output[i] << std::endl;
    }

    // Cleanup
    glDeleteBuffers(1, &inputBuffer);
    glDeleteBuffers(1, &outputBuffer);
    glDeleteProgram(prog);
    glDeleteShader(shader);
    glfwTerminate();

    return 0;
}
