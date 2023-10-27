#include "window.h"
#include "shaders.h"
#include "object.h"
#include "texture.h"
#include "camera.h"
#include "physics.h"
#include <time.h>
#include "../third_party/cglm/include/cglm/cglm.h"

int main(void)
{
	GLFWwindow *window = create_window(1920, 1080, 1, 1);
	if (window == 0)
	{
		return -1;
	}
	init_programs();

	GLuint indices[] = {
		5, 1, 0, 6, 1, 5,
		6, 2, 1, 7, 2, 6,
		7, 3, 2, 8, 3, 7,
		8, 4, 3, 9, 4, 8,
		0, 11, 10, 1, 11, 0,
		12, 6, 5, 13, 6, 12};

	GLfloat vertices[] = {
		-1, -1, -1, 0, 0,
		1, -1, -1, 1, 0,
		1, 1, -1, 2, 0,
		-1, 1, -1, 3, 0,
		-1, -1, -1, 4, 0,

		-1, -1, 1, 0, 1,
		1, -1, 1, 1, 1,
		1, 1, 1, 2, 1,
		-1, 1, 1, 3, 1,
		-1, -1, 1, 4, 1,

		-1, 1, -1, 0, -1,
		1, 1, -1, 1, -1,

		-1, 1, 1, 0, 2,
		1, 1, 1, 1, 2};

	object *cube = create_object(vertices, 14, GL_STATIC_DRAW, indices, 36, 1, 1, 1, 10, 0.1f, 0.5f);
	object *cube2 = create_object_copy(cube, 1);
	cube2->phy->priority = 3;
	scale_object(cube2, (vec3){6, 1.5f, 6}, 1);
	translate_object(cube2, (vec3){0, -10, 0}, 1);
	texture *text = load_texture("./textures/brick.jpg", GL_TEXTURE_2D, GL_UNSIGNED_BYTE, GL_NEAREST, GL_NEAREST);

	camera *cam = create_camera(1920, 1080, (vec3){0.0f, -5.0f, 15.0f}, 60, 0.1f, 100, 1, 100, -15, (vec3){1, 0, 0});

	clock_t timer = 0;
	while (!glfwWindowShouldClose(window))
	{
		timer = clock();
		glfwPollEvents();
		run_input_free_camera(cam, window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		use_texture(text);
		use_object(cube, get_def_tex_program(), cam);
		use_object(cube2, get_def_tex_program(), cam);
		glfwSwapBuffers(window);
		simulate_physic((vec3){0, -9.8f, 0}, 0.01f);
		while ((clock() / (CLOCKS_PER_SEC / 1000)) - (timer / (CLOCKS_PER_SEC / 1000)) < 16)
			;
	}

	delete_camera(cam);
	delete_object(cube);
	delete_object(cube2);
	delete_texture(text);
	destroy_programs();
	delete_window(window);

	return 0;
}