// Create compute shader opengl program to do convolution
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>
#include <random>

using namespace std;

// Global variables
GLFWwindow* window;
GLuint vao;
GLuint vbo;
GLuint ebo;
GLuint program;
GLuint compute_program;

// Constants
const char* computeShaderSource = "#version 430 core\n"
"layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
"layout(std430, binding = 0) buffer InputBuffer\n"
"{\n"
"    float input[];\n"
"};\n"
"layout(std430, binding = 1) buffer OutputBuffer\n"
"{\n"
"    float output[];\n"
"};\n"
"layout(std430, binding = 2) buffer KernelBuffer\n"
"{\n"
"    float kernel[];\n"
"};\n"
"layout(std430, binding = 3) buffer SizeBuffer\n"
"{\n"
"    int size;\n"
"};\n"
"void main()\n"
"{\n"
"    int index = int(gl_GlobalInvocationID.x);\n"
"    int kernel_size = size;\n"
"    int kernel_radius = kernel_size / 2;\n"
"    float sum = 0.0f;\n"
"    for (int i = -kernel_radius; i <= kernel_radius; i++)\n"
"    {\n"
"        sum += input[index + i] * kernel[i + kernel_radius];\n"
"    }\n"
"    output[index] = sum;\n"
"}\n";

int main(){
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
  window = glfwCreateWindow(800, 600, "Convolution", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  glewExperimental = GL_TRUE;
  glewInit();
  glViewport(0, 0, 800, 600);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPos(window, 400, 300);
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // Create vao, vbo, ebo
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  // Create compute shader program
  compute_program = glCreateProgram();
  GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
  glShaderSource(compute_shader, 1, &computeShaderSource, NULL);
  glCompileShader(compute_shader);
  glAttachShader(compute_program, compute_shader);
  glLinkProgram(compute_program);
  // set up input buffer
  const int input_size = 100;
  float input[input_size];
  for (int i = 0; i < input_size; i++)
  {
    input[i] = 1.0f;
  }
  GLuint input_buffer;
  glGenBuffers(1, &input_buffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, input_buffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(input), input, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input_buffer);
  // set up output buffer
  float output[input_size];
  GLuint output_buffer;
  glGenBuffers(1, &output_buffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, output_buffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(output), output, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, output_buffer);
  // set up kernel buffer
  const int kernel_size = 5;
  float kernel[kernel_size];
  for (int i = 0; i < kernel_size; i++)
  {
    kernel[i] = 1.0f / kernel_size;
  }
  GLuint kernel_buffer;
  glGenBuffers(1, &kernel_buffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, kernel_buffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(kernel), kernel, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, kernel_buffer);
  // set up size buffer
  int size = kernel_size;
  GLuint size_buffer;
  glGenBuffers(1, &size_buffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, size_buffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(size), &size, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, size_buffer);
  // set up output
  float output2[input_size];
  GLuint output_buffer2;
  glGenBuffers(1, &output_buffer2);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, output_buffer2);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(output2), output2, GL_STATIC_DRAW);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, output_buffer2);
  // set up kernel2 buffer
  float kernel2[kernel_size];
  for (int i = 0; i < kernel_size; i++)
  {
    kernel2[i] = 1.0f / kernel_size;
  }
  GLuint kernel_buffer2;
  
  
}