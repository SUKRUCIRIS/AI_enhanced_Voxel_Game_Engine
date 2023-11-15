#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "../../third_party/glfw/include/GLFW/glfw3.h"

GLFWwindow *create_window(int width, int height, unsigned char is_full, unsigned char vsync, int msaa);

void delete_window(GLFWwindow *window);