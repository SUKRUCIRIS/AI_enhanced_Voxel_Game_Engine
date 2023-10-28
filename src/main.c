#include "./core/window.h"
#include "./core/shaders.h"
#include "./core/object.h"
#include "./core/texture.h"
#include "./core/camera.h"
#include "./core/physics.h"
#include "./core/lighting.h"
#include <time.h>
#include "../third_party/cglm/include/cglm/cglm.h"
#include "./game/world.h"
#include <stdlib.h>

int main(void)
{
	GLFWwindow *window = create_window(1920, 1080, 1, 1);
	if (window == 0)
	{
		return -1;
	}
	init_programs();
	init_world_textures();

	camera *cam = create_camera(1920, 1080, (vec3){0.0f, 5, 0.0f}, 60, 0.1f, 100, 1, 100, -15, (vec3){1, 0, 0});
	lighting *light = create_lighting();
	light->ambient = 0.2f;
	light->lightColor[0] = 0.5294f;
	light->lightColor[1] = 0.8078f;
	light->lightColor[2] = 0.9216f;
	light->lightColor[3] = 1;
	light->lightDir[0] = -1;
	light->lightDir[1] = -1;
	light->lightDir[2] = 0;

	srand((unsigned int)time(0));
	int **hm = create_heightmap(20, 20, 3, rand(), rand());
	DA *world_cubes = create_world_cubes(hm, 20, 20);
	free(hm);
	clock_t timer = 0;
	while (!glfwWindowShouldClose(window))
	{
		timer = clock();
		glfwPollEvents();
		run_input_free_camera(cam, window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(get_def_tex_light_program());
		use_lighting(light, get_def_tex_light_program());
		use_world(world_cubes, get_def_tex_light_program(), cam);
		glfwSwapBuffers(window);
		simulate_physic((vec3){0, -9.8f, 0}, 0.01f);
		while ((clock() / (CLOCKS_PER_SEC / 1000)) - (timer / (CLOCKS_PER_SEC / 1000)) < 16)
			;
	}

	delete_camera(cam);
	delete_world_textures();
	delete_DA(world_cubes);
	delete_lighting(light);

	delete_all_object();
	delete_all_physic();
	destroy_programs();
	delete_window(window);

	return 0;
}