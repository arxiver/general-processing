#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
// Make a compute shader for a simple multiply-add operation of two vectors
const char *computeShaderSource = "#version 430 core\n"
"layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
"layout (std430, binding = 0) buffer Input {\n"
"    float a[];\n"
"    float b[];\n"
"} ainput;\n"
"layout (std430, binding = 1) buffer Output {\n"
"    float c[];\n"
"} aoutput;\n"
"void main() {\n"
"    uint index = gl_GlobalInvocationID.x;\n"
"    aoutput.c[index] = ainput.a[index] * ainput.b[index] + 1.0;\n"
"}\n";

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow *window = glfwCreateWindow(640, 480, "Compute Shader", nullptr, nullptr);
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

    // Create a compute shader
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, nullptr);
    glCompileShader(computeShader);

    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(computeShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(computeShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPUTE::COMPILATION_FAILED" << std::endl << infoLog << std::endl;
    }
    // Create the compute program
    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    // Check for linking errors
    glGetProgramiv(computeProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(computeProgram, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED" << std::endl << infoLog << std::endl;
    }
    // set up data
    const int N = 10;
    float a[N], b[N], c[N];
    for (int i = 0; i < N; i++) {
        a[i] = i;
        b[i] = i;
    }
    // create buffers
    GLuint ainputBuffer, aoutputBuffer;
    glGenBuffers(1, &ainputBuffer);
    glGenBuffers(1, &aoutputBuffer);
    // bind buffers
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ainputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 2 * N * sizeof(float), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, aoutputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, N * sizeof(float), nullptr, GL_STATIC_DRAW);
    // copy data to buffers
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ainputBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, N * sizeof(float), a);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, N * sizeof(float), N * sizeof(float), b);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, aoutputBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, N * sizeof(float), c);
    // bind buffers to binding points
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ainputBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, aoutputBuffer);
    // use the program
    glUseProgram(computeProgram);
    // run the compute shader
    glDispatchCompute(N, 1, 1);
    // wait for the compute shader to finish
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    // copy the result back to the host
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, aoutputBuffer);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, N * sizeof(float), c);
    // print the result
    for (int i = 0; i < N; i++) {
        std::cout << c[i] << std::endl;
    }
    // clean up
    glDeleteBuffers(1, &ainputBuffer);
    glDeleteBuffers(1, &aoutputBuffer);
    glDeleteProgram(computeProgram);
    glDeleteShader(computeShader);
    glfwTerminate();
    return 0;
}

