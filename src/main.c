#include "./core/core.h"
#include <time.h>
#include "./game/world_batch.h"
#include "./game/world_instanced.h"

#define USE_INSTANCED 0

int main(void)
{
	GLFWwindow *window = create_window(1920, 1080, 1, 1, 4);
	if (window == 0)
	{
		return -1;
	}

	init_programs();
	float render_distance = 1000;
	int world_size = 1000;
	camera *cam = create_camera(1920, 1080, (vec3){0.0f, 5, 0.0f}, 60, 0.1f, render_distance, 1, 100, -15, (vec3){1, 0, 0});
	lighting *light = create_lighting(window, cam, 8192, 8192, render_distance / 50, render_distance / 25, render_distance / 10, render_distance / 2);

	srand((unsigned int)time(0));
	int **hm = create_heightmap(world_size, world_size, 100, rand(), rand(), 175);
#if USE_INSTANCED == 0
	world_batch *world_cubes = create_world_batch(hm, world_size, world_size);
	br_scene gsu = load_object_br(world_cubes->obj_manager, world_cubes->tex_manager, "./models/gsu.fbx", 2, 1, 0, 3, 10, 0.1f, 0.5f);
	for (unsigned int i = 0; i < gsu.mesh_count; i++)
	{
		scale_br_object(gsu.meshes[i], (vec3){0.01f, 0.01f, 0.01f}, 0);
		translate_br_object(gsu.meshes[i], (vec3){0, 100, 0}, 0);
	}
	prepare_render_br_object_manager(world_cubes->obj_manager);
#elif USE_INSTANCED == 1
	world_instanced *world_cubes = create_world_instanced(hm, world_size, world_size);
#endif
	free(hm);
	clock_t timer = 0;
	clock_t frame_ms = 0;
	while (!glfwWindowShouldClose(window))
	{
		timer = clock();
		glfwPollEvents();
		run_input_free_camera(cam, window);
#if USE_INSTANCED == 0
		glUseProgram(get_def_shadowmap_br_program());
		use_lighting(light, get_def_shadowmap_br_program(), 1);
		use_world_batch(world_cubes, get_def_shadowmap_br_program());

		glUseProgram(get_def_tex_light_br_program());
		use_lighting(light, get_def_tex_light_br_program(), 0);
		use_camera(cam, get_def_tex_light_br_program());
		use_world_batch(world_cubes, get_def_tex_light_br_program());
#elif USE_INSTANCED == 1
		glUseProgram(get_def_shadowmap_ins_program());
		use_lighting(light, get_def_shadowmap_ins_program(), 1);
		use_world_instanced(world_cubes, get_def_shadowmap_ins_program());

		glUseProgram(get_def_tex_light_ins_program());
		use_lighting(light, get_def_tex_light_ins_program(), 0);
		use_camera(cam, get_def_tex_light_ins_program());
		use_world_instanced(world_cubes, get_def_tex_light_ins_program());
#endif

		glfwSwapBuffers(window);

		simulate_physic((vec3){0, -9.8f, 0}, 0.01f);

		frame_ms = (clock() - timer) / (CLOCKS_PER_SEC / 1000);
	}

	delete_camera(cam);
#if USE_INSTANCED == 0
	delete_world_batch(world_cubes);
#elif USE_INSTANCED == 1
	delete_world_instanced(world_cubes);
#endif
	delete_lighting(light);

	delete_all_physic();
	destroy_programs();
	delete_window(window);

	return 0;
}