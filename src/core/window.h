#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "../../third_party/glfw/include/GLFW/glfw3.h"

GLFWwindow *create_window(int width, int height, unsigned char is_full, unsigned char vsync, int msaa);

void delete_window(GLFWwindow *window);

unsigned char get_key_pressed(int key);

unsigned char get_key_released(int key);

unsigned char get_key_down(int key);

unsigned char get_key_up(int key);

void poll_events(GLFWwindow *window);