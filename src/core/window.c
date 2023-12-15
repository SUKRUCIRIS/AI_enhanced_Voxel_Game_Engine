#include "window.h"
#include <locale.h>

// make the computer use best gpu
#ifdef _MSC_VER
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

GLFWwindow *create_window(int width, int height, unsigned char is_full, unsigned char vsync, int msaa)
{
	setlocale(LC_ALL, "Turkish"); // for turkish characters
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, msaa);
	if (width <= 0 || height <= 0)
	{
		const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
		width = mode->width;
		height = mode->height;
	}
	GLFWwindow *window = 0;
	if (is_full)
	{
		window = glfwCreateWindow(width, height, "Şükrü Çiriş Engine", glfwGetPrimaryMonitor(), 0);
	}
	else
	{
		window = glfwCreateWindow(width, height, "Şükrü Çiriş Engine", 0, 0);
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