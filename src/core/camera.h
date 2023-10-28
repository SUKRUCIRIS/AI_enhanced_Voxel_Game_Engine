#pragma once
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "../../third_party/opengl/include/glad/glad.h"
#include "../../third_party/glfw/include/GLFW/glfw3.h"
#include "dynamic.h"

typedef struct camera
{
	vec3 position;
	vec3 orientation;
	vec3 up;
	unsigned char firstclick;
	int width;
	int height;
	float speed;
	float sensitivity;
	mat4 result;
	float FOVdeg;
	float nearPlane;
	float farPlane;
	mat4 view;
	mat4 projection;
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
} camera;

camera *create_camera(int width, int height, vec3 position, float FOVdeg, float nearPlane, float farPlane, float speed,
					  float sensitivity, float angle, vec3 angle_axis);

void delete_camera(camera *cam);

void run_input_free_camera(camera *cam, GLFWwindow *window);

void calculate_camera(camera *cam);

void use_camera(camera *cam, GLuint program);