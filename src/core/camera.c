#include "camera.h"

camera *create_camera(int width, int height, vec3 position, float FOVdeg, float nearPlane, float farPlane, float speed,
											float sensitivity, float angle, vec3 angle_axis)
{
	camera *cam = calloc(1, sizeof(camera));
	cam->orientation[0] = 0;
	cam->orientation[1] = 0;
	cam->orientation[2] = -1;
	cam->up[0] = 0;
	cam->up[1] = 1;
	cam->up[2] = 0;
	cam->firstclick = 1;
	cam->speed = speed;
	cam->sensitivity = sensitivity;
	cam->width = width;
	cam->height = height;
	cam->position[0] = position[0];
	cam->position[1] = position[1];
	cam->position[2] = position[2];
	cam->FOVdeg = FOVdeg;
	cam->nearPlane = nearPlane;
	cam->farPlane = farPlane;
	glm_vec3_rotate(cam->orientation, glm_rad(angle), angle_axis);
	glm_mat4_identity(cam->view);
	glm_mat4_identity(cam->projection);
	cam->programs = create_DA(sizeof(GLuint));
	cam->uniforms = create_DA(sizeof(GLint));
	return cam;
}

void delete_camera(camera *cam)
{
	delete_DA(cam->programs);
	delete_DA(cam->uniforms);
	free(cam);
}

void run_input_fp_camera(camera *cam, GLFWwindow *window)
{
	if (cam->firstclick)
	{
		glfwSetCursorPos(window, (cam->width / 2), (cam->height / 2));
		cam->firstclick = 0;
	}
	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	float rotX = cam->sensitivity * (float)(mouseY - (cam->height / 2)) / cam->height;
	float rotY = cam->sensitivity * (float)(mouseX - (cam->width / 2)) / cam->width;
	vec3 oldor;
	oldor[0] = cam->orientation[0];
	oldor[1] = cam->orientation[1];
	oldor[2] = cam->orientation[2];
	vec3 cross;
	glm_vec3_cross(cam->orientation, cam->up, cross);
	glm_vec3_normalize(cross);
	glm_vec3_rotate(cam->orientation, glm_rad(-rotX), cross);
	if (fabs(glm_vec3_angle(cam->orientation, cam->up) - glm_rad(90.0f)) > glm_rad(85.0f))
	{
		cam->orientation[0] = oldor[0];
		cam->orientation[1] = oldor[1];
		cam->orientation[2] = oldor[2];
	}
	glm_vec3_rotate(cam->orientation, glm_rad(-rotY), cam->up);
	glfwSetCursorPos(window, (cam->width / 2), (cam->height / 2));
}

void run_input_free_camera(camera *cam, GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		vec3 direction;
		glm_vec3_scale(cam->orientation, cam->speed, direction);
		glm_vec3_add(cam->position, direction, cam->position);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		vec3 direction;
		vec3 cross;
		glm_vec3_cross(cam->orientation, cam->up, cross);
		glm_normalize_to(cross, cross);
		glm_vec3_scale(cross, -(cam->speed), direction);
		glm_vec3_add(cam->position, direction, cam->position);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		vec3 direction;
		glm_vec3_scale(cam->orientation, -(cam->speed), direction);
		glm_vec3_add(cam->position, direction, cam->position);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		vec3 direction;
		vec3 cross;
		glm_vec3_cross(cam->orientation, cam->up, cross);
		glm_normalize_to(cross, cross);
		glm_vec3_scale(cross, cam->speed, direction);
		glm_vec3_add(cam->position, direction, cam->position);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		vec3 direction;
		glm_vec3_scale(cam->up, cam->speed, direction);
		glm_vec3_add(cam->position, direction, cam->position);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		vec3 direction;
		glm_vec3_scale(cam->up, -cam->speed, direction);
		glm_vec3_add(cam->position, direction, cam->position);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		cam->speed = 1;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		cam->speed = 0.1f;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		run_input_fp_camera(cam, window);
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		cam->firstclick = 1;
	}
}

void calculate_camera(camera *cam, float near_plane, float far_plane)
{
	vec3 sum;
	glm_vec3_add(cam->position, cam->orientation, sum);
	glm_lookat(cam->position, sum, cam->up, cam->view);
	glm_perspective(glm_rad(cam->FOVdeg), (float)cam->width / cam->height, near_plane, far_plane, cam->projection);
	glm_mat4_mul(cam->projection, cam->view, cam->result);
}

void use_camera(camera *cam, GLuint program)
{
	calculate_camera(cam, cam->nearPlane, cam->farPlane);
	if (get_index_DA(cam->programs, &program) == UINT_MAX)
	{
		pushback_DA(cam->programs, &program);
		GLint uniform = glGetUniformLocation(program, "camera");
		pushback_DA(cam->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "camPos");
		pushback_DA(cam->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "view");
		pushback_DA(cam->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "projection");
		pushback_DA(cam->uniforms, &uniform);
	}
	GLint *uniforms = get_data_DA(cam->uniforms);
	glUniformMatrix4fv(uniforms[get_index_DA(cam->programs, &program) * 4], 1, GL_FALSE, cam->result[0]);
	glUniform3f(uniforms[get_index_DA(cam->programs, &program) * 4 + 1], cam->position[0], cam->position[1], cam->position[2]);
	glUniformMatrix4fv(uniforms[get_index_DA(cam->programs, &program) * 4 + 2], 1, GL_FALSE, cam->view[0]);
	glUniformMatrix4fv(uniforms[get_index_DA(cam->programs, &program) * 4 + 3], 1, GL_FALSE, cam->projection[0]);
}