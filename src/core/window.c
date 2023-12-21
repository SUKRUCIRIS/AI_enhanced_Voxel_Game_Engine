#include "window.h"
#include <locale.h>

// make the computer use best gpu
#ifdef _MSC_VER
__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

unsigned char pressed[GLFW_KEY_LAST + 1] = {0};
unsigned char previous[GLFW_KEY_LAST + 1] = {0};
unsigned char released[GLFW_KEY_LAST + 1] = {0};
unsigned char up[GLFW_KEY_LAST + 1] = {0};
unsigned char down[GLFW_KEY_LAST + 1] = {0};

unsigned char get_key_pressed(int key)
{
	return pressed[key];
}

unsigned char get_key_released(int key)
{
	return released[key];
}

unsigned char get_key_down(int key)
{
	return down[key];
}

unsigned char get_key_up(int key)
{
	return up[key];
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (previous[key] == 0)
		{
			pressed[key] = 1;
		}
		else if (previous[key] == 1)
		{
			pressed[key] = 0;
		}
		previous[key] = 1;
		released[key] = 0;
		up[key] = 1;
		down[key] = 0;
	}
	else if (action == GLFW_RELEASE)
	{
		if (previous[key] == 0)
		{
			released[key] = 0;
		}
		else if (previous[key] == 1)
		{
			released[key] = 1;
		}
		previous[key] = 0;
		pressed[key] = 0;
		up[key] = 0;
		down[key] = 1;
	}
}

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
	glfwSetKeyCallback(window, key_callback);
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