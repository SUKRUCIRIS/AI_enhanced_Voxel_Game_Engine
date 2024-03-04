#include "./core/core.h"
#include "./game/chunk.h"
#include "./game/world_batch.h"

int main(void)
{
	GLFWwindow *window = create_window(0, 0, 1, 1, 0);
	if (window == 0)
	{
		return -1;
	}

	random_seed(get_timems());

	init_programs();
	init_animations();
	init_jolt();

	int world_size = 2048;
	int chunk_size = 16;
	int chunk_range = 32;
	float render_distance = (float)chunk_size * chunk_range * 1.5f;
	float fog_start = ((float)chunk_size - 2) * chunk_range;
	float fog_end = (float)chunk_size * chunk_range;
	// vec3 fair_fog_color = {0.718f, 0.702f, 0.671f};
	vec3 dark_fog_color = {0.0718f, 0.0702f, 0.0671f};

	int window_w = 0, window_h = 0;
	glfwGetWindowSize(window, &window_w, &window_h);
	camera *cam = create_camera(window_w, window_h, (vec3){0.0f, 5, 60.0f}, 60, 0.1f, render_distance, 1, 100, -15, (vec3){1, 0, 0});

	lighting *light = create_lighting(window, cam, 4096, 4096, render_distance / 64, render_distance / 16,
																		render_distance / 4, render_distance, fog_start, fog_end, dark_fog_color, 1);
	light->fxaa = 1;
	light->vignette_pp = 1;

	DA *points = create_DA(sizeof(float));
	DA *heights = create_DA(sizeof(int));
	float tmp[] = {0, 0.30f, 0.34f, 0.37f, 0.41f, 0.44f, 0.46f, 0.48f, 0.49f, 0.51f, 0.52f, 0.54f, 0.57f, 0.60f, 0.64f, 0.67f, 0.71f, 1};
	pushback_many_DA(points, tmp, 18);
	int tmpi[] = {0, 35, 36, 47, 50, 50, 50, 52, 57, 64, 75, 85, 91, 93, 94, 94, 96, 100};
	pushback_many_DA(heights, tmpi, 18);

	int seedx = rand();
	int seedz = rand();

	int **hm = create_heightmap(world_size, world_size, seedx, seedz, 600, 0, 0, 0, 3, 2, 3, 0.3f, 30);

	delete_DA(points);
	delete_DA(heights);

	struct aiScene *gsu_model = load_model("./models/gsu.fbx", 1);
	set_gsu_model(gsu_model);

	player sukru;
	sukru.speed = 0.005f;
	sukru.jumping = 0;
	sukru.onland = 1;
	sukru.fp_camera = cam;
	sukru.width = 1;
	sukru.height = 3;
	sukru.hm = hm;
	sukru.dimensionx = world_size;
	sukru.dimensionz = world_size;
	sukru.jumpdurationms = 100;
	chunk_op *chunks = create_chunk_op(chunk_size, chunk_range, &sukru, hm, world_size, world_size, 0);
	unsigned char freec = 0;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	text_manager *t = create_text_manager("./fonts/arial.ttf", 16, 1920, 1080, GL_LINEAR, GL_LINEAR);

	float width, height;
	vec4 red = {1, 0, 0, 1};
	get_text_size(t, 1, "Sukru Ciris Engine", &width, &height);
	width = 1920 - width;
	height = 1080 - height;
	add_text(t, width, height, 1, 1, red, "Sukru Ciris Engine");

	skybox *s = create_skybox("./textures/skybox/eso/right.png",
														"./textures/skybox/eso/left.png",
														"./textures/skybox/eso/top.png",
														"./textures/skybox/eso/bottom.png",
														"./textures/skybox/eso/front.png",
														"./textures/skybox/eso/back.png",
														cam, 0.0001f, (vec3){0, 1, 0});

	while (!glfwWindowShouldClose(window))
	{
		start_game_loop();

		{
			clear_text_manager(t);

			get_text_size(t, 1, "Sukru Ciris Engine", &width, &height);
			width = 1920 - width;
			height = 1080 - height;
			add_text(t, width, height, 1, 1, red, "Sukru Ciris Engine");

			get_text_size_variadic(t, 1, &width, &height, "Frame: %.2lf ms\nSeedx: %d\nSeedz: %d", get_frame_timems(), seedx, seedz);
			width = 0;
			height = 1080 - height;
			add_text_variadic(t, width, height, 1, 1, red, "Frame: %.2lf ms\nSeedx: %d\nSeedz: %d", get_frame_timems(), seedx, seedz);
		}

		glfwPollEvents();
		poll_events(window);

		update_chunk_op(chunks);
		play_animations();

		if (get_key_pressed(GLFW_KEY_K) == 1)
		{
			if (freec == 0)
			{
				freec = 1;
			}
			else
			{
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				freec = 0;
			}
		}
		if (freec == 0)
		{
			run_input_player(&sukru, window, get_frame_timems());
		}
		else
		{
			run_input_free_camera(cam, window);
		}

		glUseProgram(get_def_shadowmap_br_program());
		use_lighting_shadowpass(light, get_def_shadowmap_br_program());
		use_chunk_op(chunks, get_def_shadowmap_br_program(), cam);

		glUseProgram(get_def_gbuffer_br_program());
		use_lighting_gbuffer(light, get_def_gbuffer_br_program());
		use_chunk_op(chunks, get_def_gbuffer_br_program(), cam);
		glUseProgram(get_def_skybox_program());
		use_skybox(s, get_def_skybox_program());

		glUseProgram(get_def_ssao_program());
		use_lighting_ssao(light, get_def_ssao_program());

		glUseProgram(get_def_ssao_blur_program());
		use_lighting_ssao_blur(light, get_def_ssao_blur_program());

		glUseProgram(get_def_deferred_br_program());
		use_lighting_deferred(light, get_def_deferred_br_program());

		glUseProgram(get_def_post_process_program());
		use_lighting_postprocess(light, get_def_post_process_program());

		glUseProgram(get_def_text_program());
		use_text_manager(t, get_def_text_program());

		glfwSwapBuffers(window);

		if (get_key_pressed(GLFW_KEY_F))
		{
			light->fxaa = 1 - light->fxaa;
		}

		end_game_loop_targetms(16.5);
	}

	delete_camera(cam);
	delete_lighting(light);

	delete_chunk_op(chunks);

	delete_all_physic();
	destroy_programs();
	delete_window(window);
	delete_animations();
	delete_text_manager(t);
	delete_skybox(s);
	deinit_jolt();

	free(hm);
	free_model(gsu_model);

	return 0;
}