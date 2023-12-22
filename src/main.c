#include "./core/core.h"
#include "./game/chunk.h"

int main(void)
{
	GLFWwindow *window = create_window(0, 0, 1, 1, 4);
	if (window == 0)
	{
		return -1;
	}

	init_programs();

	int world_size = 2000;
	int chunk_size = 32;
	int chunk_range = 5;
	float render_distance = (float)chunk_size * chunk_range * 2;

	int window_w = 0, window_h = 0;
	glfwGetWindowSize(window, &window_w, &window_h);
	camera *cam = create_camera(window_w, window_h, (vec3){0.0f, 5, 0.0f}, 60, 0.1f, render_distance, 1, 100, -15, (vec3){1, 0, 0});

	lighting *light = create_lighting(window, cam, 8192, 8192, render_distance / 64, render_distance / 16, render_distance / 4, render_distance);

	int **hm = create_heightmap(world_size, world_size, 100, 1453, 1071, 175, 100);
	// world_batch *world_cubes = create_world_batch(hm, 0, 0, 1000, 1000, world_size, world_size);
	// struct aiScene *gsu_model = load_model("./models/gsu.fbx", 1);
	// br_scene gsu = load_object_br(world_cubes->obj_manager, get_world_texture_manager(), gsu_model, 2, 0, 3, 10, 0.1f, 0.5f);
	// free_model(gsu_model);
	// for (unsigned int i = 0; i < gsu.mesh_count; i++)
	//{
	// scale_br_object(gsu.meshes[i], (vec3){0.01f, 0.01f, 0.01f}, 0);
	// translate_br_object(gsu.meshes[i], (vec3){0, 100, 0}, 0);
	//}
	// prepare_render_br_object_manager(world_cubes->obj_manager);
	player sukru;
	sukru.speed = 0.001f;
	sukru.jumping = 0;
	sukru.fp_camera = cam;
	sukru.maxaabb[0] = 0.5f;
	sukru.maxaabb[1] = 0.5f;
	sukru.maxaabb[2] = 0.5f;
	sukru.minaabb[0] = -0.5f;
	sukru.minaabb[1] = -1.5f;
	sukru.minaabb[2] = -0.5f;
	chunk_op *chunks = create_chunk_op(chunk_size, chunk_range, &sukru, hm, world_size, world_size);

	while (!glfwWindowShouldClose(window))
	{
		start_game_loop();

		glfwPollEvents();
		update_chunk_op(chunks);

		run_input_player(&sukru, window, get_frame_timems());

		glUseProgram(get_def_shadowmap_br_program());
		use_lighting(light, get_def_shadowmap_br_program(), 1);
		use_chunk_op(chunks, get_def_shadowmap_br_program());

		glUseProgram(get_def_tex_light_br_program());
		use_lighting(light, get_def_tex_light_br_program(), 0);
		use_camera(cam, get_def_tex_light_br_program());
		use_chunk_op(chunks, get_def_tex_light_br_program());

		glfwSwapBuffers(window);

		simulate_physic((vec3){0, -9.8f, 0}, 0.01f);

		end_game_loop();
	}

	delete_camera(cam);
	delete_lighting(light);

	delete_chunk_op(chunks);

	delete_all_physic();
	destroy_programs();
	delete_window(window);

	free(hm);

	return 0;
}