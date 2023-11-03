#include "window.h"

GLFWwindow *create_window(int width, int height, unsigned char is_full, unsigned char vsync)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	GLFWwindow *window = 0;
	if (is_full)
	{
		window = glfwCreateWindow(width, height, "Red Crescent Engine", glfwGetPrimaryMonitor(), 0);
	}
	else
	{
		window = glfwCreateWindow(width, height, "Red Crescent Engine", 0, 0);
	}
	if (window == 0)
	{
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);
	if (vsync)
	{
		glfwSwapInterval(1);
	}
	else
	{
		glfwSwapInterval(0);
	}
	gladLoadGL();
	glViewport(0, 0, width, height);
	glClearColor(0.5294f, 0.8078f, 0.9216f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);
	return window;
}

void delete_window(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}