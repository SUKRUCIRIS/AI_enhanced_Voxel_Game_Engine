#include "gameloop.h"

void gameloop(GLFWwindow *window, int **hm, int seedx, int seedz, int dimensionx, int dimensionz)
{
  init_animations();
  float gravity[3] = {0, -10, 0};
  init_jolt(gravity);

  int chunk_size = 16;
  int chunk_range = 32;
  float render_distance = (float)chunk_size * chunk_range * 1.5f;
  float fog_start = ((float)chunk_size - 2) * chunk_range;
  float fog_end = (float)chunk_size * chunk_range;
  // vec3 fair_fog_color = {0.718f, 0.702f, 0.671f};
  vec3 dark_fog_color = {0.0718f, 0.0702f, 0.0671f};

  int window_w = 0, window_h = 0;
  glfwGetWindowSize(window, &window_w, &window_h);
  camera *cam = create_camera(window_w, window_h, (vec3){0.0f, 5, 60.0f}, 60, 0.1f, render_distance, 1, 100, -90, (vec3){0, 1, 0});

  lighting *light = create_lighting(window, cam, 4096, 4096, render_distance / 64, render_distance / 16,
                                    render_distance / 4, render_distance, fog_start, fog_end, dark_fog_color, 1);
  light->fxaa = 1;
  light->vignette_pp = 1;

  struct aiScene *gsu_model = load_model("./models/gsu.fbx", 1);
  set_gsu_model(gsu_model);

  float startpos[3] = {150, (float)hm[dimensionx / 2 + 150][dimensionz / 2] + 5.0f, 0};
  player *p = create_player(cam, 3, 5, 0.75f, 2, 0.8f, 2, hm, dimensionx, dimensionz, "./models/player.fbx", startpos, 80, 100, 70, 1);

  chunk_op *chunks = create_chunk_op(chunk_size, chunk_range, p, hm, dimensionx, dimensionz, 0);
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
                            cam, 0.00005f, (vec3){1, 1, 1});
  bodyid *hm_boxes = create_hm_voxel_jolt(hm, dimensionx, dimensionz, 0, 0, dimensionx, dimensionz, 0.2f, 0.2f, 1);
  optimize_jolt();
  while (!glfwWindowShouldClose(window))
  {
    start_game_loop();

    {
      clear_text_manager(t);

      get_text_size(t, 1, "Sukru Ciris Engine", &width, &height);
      width = 1920 - width;
      height = 1080 - height;
      add_text(t, width, height, 1, 1, red, "Sukru Ciris Engine");

      get_text_size(t, 1, "AI Enhanced Voxel Game Engine", &width, &height);
      width = 1920 - width;
      height = 1060 - height;
      add_text(t, width, height, 1, 1, red, "AI Enhanced Voxel Game Engine");

      get_gravity_jolt(gravity);

      if (seedx != -1 || seedz != -1)
      {
        get_text_size_variadic(t, 1, &width, &height, "Frame: %.2lf ms\nSeedx: %d\nSeedz: %d\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA",
                               get_frame_timems(), seedx, seedz, get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2]);
        width = 0;
        height = 1080 - height;
        add_text_variadic(t, width, height, 1, 1, red, "Frame: %.2lf ms\nSeedx: %d\nSeedz: %d\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA",
                          get_frame_timems(), seedx, seedz, get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2]);
      }
      else
      {
        get_text_size_variadic(t, 1, &width, &height, "Frame: %.2lf ms\nUsing heightmap texture\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA",
                               get_frame_timems(), get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2]);
        width = 0;
        height = 1080 - height;
        add_text_variadic(t, width, height, 1, 1, red, "Frame: %.2lf ms\nUsing heightmap texture\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA",
                          get_frame_timems(), get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2]);
      }
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
      else if (freec == 1)
      {
        freec = 2;
      }
      else if (freec == 2)
      {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        freec = 0;
      }
    }
    if (freec == 0)
    {
      run_input_player(p, window, get_frame_timems(), 0);
    }
    else if (freec == 1)
    {
      run_input_player(p, window, get_frame_timems(), 1);
    }
    else if (freec == 2)
    {
      run_input_free_camera(cam, window);
    }

    glUseProgram(get_def_shadowmap_br_program());
    use_lighting_shadowpass(light, get_def_shadowmap_br_program());
    use_chunk_op(chunks, get_def_shadowmap_br_program(), cam);
    render_player(p, get_def_shadowmap_br_program());

    glUseProgram(get_def_gbuffer_br_program());
    use_lighting_gbuffer(light, get_def_gbuffer_br_program());
    use_chunk_op(chunks, get_def_gbuffer_br_program(), cam);
    render_player(p, get_def_gbuffer_br_program());
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

    run_jolt((float)get_frame_timems() / 1000.0f);
    end_game_loop_targetms(16.5);
  }

  delete_camera(cam);
  delete_lighting(light);

  delete_chunk_op(chunks);

  delete_all_physic();
  delete_animations();
  delete_text_manager(t);
  delete_skybox(s);

  delete_body_jolt(hm_boxes);
  delete_player(p);
  deinit_jolt();

  free_model(gsu_model);
}