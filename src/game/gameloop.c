#include "gameloop.h"
#include "../core/core.h"

unsigned char loading_done = 0;

typedef struct loads
{
  camera *cam;
  lighting *light;
  void *window;
  int **hm;
  int dimensionx;
  int dimensionz;
  player *p;
  chunk_op *chunks;
  text_manager *t;
  skybox *s;
  bodyid *hm_boxes;
  float sealevel;
  int chunk_range;
  int chunk_size;
  unsigned char loadgsu;
  unsigned char ssao;
  unsigned char facemerged;
} loads;

void loadres(void *ress)
{
  loads *resss = (loads *)ress;
  glfwMakeContextCurrent((GLFWwindow *)resss->window);
  int window_w = 0, window_h = 0;
  glfwGetWindowSize((GLFWwindow *)resss->window, &window_w, &window_h);
  {
    text_manager *t = create_text_manager("./fonts/arial.ttf", 128, 1920, 1080, window_w, window_h, GL_LINEAR, GL_LINEAR);
    float width, height;
    vec4 red = {1, 1, 1, 1};
    get_text_size(t, 1, "Loading...", &width, &height);
    width = (1920 - width) / 2.0f;
    height = (1080 - height) / 2.0f;
    add_text(t, width, height, 1, 1, red, "Loading...");
    get_text_size(t, 1, "Sukru Ciris Engine", &width, &height);
    width = (1920 - width) / 2.0f;
    height = (1080 - height) / 2.0f - 200;
    add_text(t, width, height, 1, 1, red, "Sukru Ciris Engine");
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(get_def_text_program());
    use_text_manager(t, get_def_text_program());
    glfwSwapBuffers((GLFWwindow *)resss->window);
    delete_text_manager(t);
  }
  float render_distance = (float)resss->chunk_size * resss->chunk_range * 1.5f;
  float fog_start = ((float)resss->chunk_size - 2) * resss->chunk_range;
  float fog_end = (float)resss->chunk_size * resss->chunk_range;
  // vec3 fair_fog_color = {0.718f, 0.702f, 0.671f};
  vec3 dark_fog_color = {0.0718f, 0.0702f, 0.0671f};

  vec3 cam_pos = {0.0f, 5, 60.0f};
  vec3 angle_axis = {0, 1, 0};
  resss->cam = create_camera(window_w, window_h, cam_pos, 60, 0.1f, render_distance, 1, 100, -90, angle_axis);

  resss->light = create_lighting((GLFWwindow *)resss->window, resss->cam, 4096, 4096, render_distance / 64, render_distance / 16,
                                 render_distance / 4, render_distance, fog_start, fog_end, dark_fog_color, 1, resss->ssao);
  resss->light->fxaa = 1;
  resss->light->vignette_pp = 1;

  struct aiScene *gsu_model = 0;
  if (resss->loadgsu)
  {
    gsu_model = load_model("./models/gsu.fbx", 1);
    set_gsu_model(gsu_model);
  }

  float startpos[3] = {0, max((float)resss->hm[resss->dimensionx / 2][resss->dimensionz / 2], resss->sealevel) + 5.0f, 0};
  resss->p = create_player(resss->cam, 3, 5, 0.75f, 2, 0.8f, 2, resss->hm, resss->dimensionx, resss->dimensionz,
                           "./models/player.fbx", startpos, 80, 100, 70, 1);

  resss->chunks = create_chunk_op(resss->chunk_size, resss->chunk_range, resss->p, resss->hm,
                                  resss->dimensionx, resss->dimensionz, 0, resss->sealevel, resss->facemerged);

  resss->t = create_text_manager("./fonts/arial.ttf", 16, 1920, 1080, window_w, window_h, GL_LINEAR, GL_LINEAR);

  float width, height;
  vec4 red = {1, 0, 0, 1};
  get_text_size(resss->t, 1, "Sukru Ciris Engine", &width, &height);
  width = 1920 - width;
  height = 1080 - height;
  add_text(resss->t, width, height, 1, 1, red, "Sukru Ciris Engine");

  vec3 rotate_axis = {1, 1, 1};
  resss->s = create_skybox("./textures/skybox/eso/right.png",
                           "./textures/skybox/eso/left.png",
                           "./textures/skybox/eso/top.png",
                           "./textures/skybox/eso/bottom.png",
                           "./textures/skybox/eso/front.png",
                           "./textures/skybox/eso/back.png",
                           resss->cam, 0.000002f, rotate_axis);
  resss->hm_boxes = create_hm_voxel_jolt(resss->hm, resss->dimensionx, resss->dimensionz, 0, 0,
                                         resss->dimensionx, resss->dimensionz, 0.2f, 0.2f, 1);
  optimize_jolt();
  free_model(gsu_model);
  glfwMakeContextCurrent(0);
  loading_done = 1;
}

void gameloop(void *window, int **hm, int seedx, int seedz, int dimensionx, int dimensionz,
              float sealevel, int chunk_range, int chunk_size, unsigned char loadgsu, unsigned char ssao,
              unsigned char facemerged, unsigned char chunkanimations)
{
  init_animations();
  float gravity[3] = {0, -10, 0};
  init_jolt(gravity);
  unsigned char freec = 0;
  loads resss;
  resss.window = window;
  resss.hm = hm;
  resss.dimensionx = dimensionx;
  resss.dimensionz = dimensionz;
  resss.sealevel = sealevel;
  resss.chunk_range = chunk_range;
  resss.chunk_size = chunk_size;
  resss.loadgsu = loadgsu;
  resss.ssao = ssao;
  resss.facemerged = facemerged;
  glfwMakeContextCurrent(0);
  Thread *load_thread = create_thread(loadres, &resss);

  {
    glfwSetInputMode((GLFWwindow *)window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    while (!glfwWindowShouldClose((GLFWwindow *)window))
    {
      glfwPollEvents();
      poll_events((GLFWwindow *)window);
      if (loading_done == 1)
      {
        join_thread(load_thread);
        break;
      }
    }
    if (loading_done == 0)
    {
      exit(-2);
    }
    glfwSetInputMode((GLFWwindow *)window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwMakeContextCurrent(window);
  }

  float width, height;
  vec4 red = {1, 0, 0, 1};

  unsigned char wireframe = 0;

  while (!glfwWindowShouldClose((GLFWwindow *)window))
  {
    start_game_loop();

    {
      clear_text_manager(resss.t);

      get_text_size(resss.t, 1, "Sukru Ciris Engine", &width, &height);
      width = 1920 - width;
      height = 1080 - height;
      add_text(resss.t, width, height, 1, 1, red, "Sukru Ciris Engine");

      get_text_size(resss.t, 1, "AI Enhanced Voxel Game Engine", &width, &height);
      width = 1920 - width;
      height = 1060 - height;
      add_text(resss.t, width, height, 1, 1, red, "AI Enhanced Voxel Game Engine");

      get_gravity_jolt(gravity);

      if (seedx != -1 || seedz != -1)
      {
        get_text_size_variadic(resss.t, 1, &width, &height, "Frame: %.2lf ms\nSeedx: %d\nSeedz: %d\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA\nPress R to disable/enable wireframe render\n\nWhole world triangle count: %d\nCurrently rendering triangle count: %d",
                               get_frame_timems(), seedx, seedz, get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2], get_world_triangle_count(), get_rendered_triangle_count());
        width = 0;
        height = 1080 - height;
        add_text_variadic(resss.t, width, height, 1, 1, red, "Frame: %.2lf ms\nSeedx: %d\nSeedz: %d\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA\nPress R to disable/enable wireframe render\n\nWhole world triangle count: %d\nCurrently rendering triangle count: %d",
                          get_frame_timems(), seedx, seedz, get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2], get_world_triangle_count(), get_rendered_triangle_count());
      }
      else
      {
        get_text_size_variadic(resss.t, 1, &width, &height, "Frame: %.2lf ms\nUsing heightmap texture\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA\nPress R to disable/enable wireframe render\n\nWhole world triangle count: %d\nCurrently rendering triangle count: %d",
                               get_frame_timems(), get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2], get_world_triangle_count(), get_rendered_triangle_count());
        width = 0;
        height = 1080 - height;
        add_text_variadic(resss.t, width, height, 1, 1, red, "Frame: %.2lf ms\nUsing heightmap texture\n\nJolt Body Count: %d\nJolt Active Body Count: %d\nJolt Gravity: {%.2lf | %.2lf | %.2lf}\n\nPress K to change camera\nPress F to disable/enable FXAA\nPress R to disable/enable wireframe render\n\nWhole world triangle count: %d\nCurrently rendering triangle count: %d",
                          get_frame_timems(), get_body_count_jolt(), get_active_body_count_jolt(), gravity[0], gravity[1], gravity[2], get_world_triangle_count(), get_rendered_triangle_count());
      }
    }

    glfwPollEvents();
    poll_events((GLFWwindow *)window);

    update_chunk_op(resss.chunks, chunkanimations);
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
        glfwSetInputMode((GLFWwindow *)window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        freec = 0;
      }
    }
    if (freec == 0)
    {
      run_input_player(resss.p, (GLFWwindow *)window, get_frame_timems(), 0);
    }
    else if (freec == 1)
    {
      run_input_player(resss.p, (GLFWwindow *)window, get_frame_timems(), 1);
    }
    else if (freec == 2)
    {
      run_input_free_camera(resss.cam, (GLFWwindow *)window);
    }
    if (get_key_pressed(GLFW_KEY_R))
    {
      if (wireframe == 0)
      {
        wireframe = 1;
      }
      else
      {
        wireframe = 0;
      }
    }

    glUseProgram(get_def_shadowmap_br_program());
    use_lighting_shadowpass(resss.light, get_def_shadowmap_br_program());
    use_chunk_op(resss.chunks, get_def_shadowmap_br_program(), resss.cam, 0);
    render_player(resss.p, get_def_shadowmap_br_program());

    if (wireframe == 1)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glUseProgram(get_def_gbuffer_br_program());
    use_lighting_gbuffer(resss.light, get_def_gbuffer_br_program(), 1);
    use_chunk_op(resss.chunks, get_def_gbuffer_br_program(), resss.cam, 0);
    render_player(resss.p, get_def_gbuffer_br_program());

    glUseProgram(get_def_water_program());
    use_lighting_gbuffer(resss.light, get_def_water_program(), 0);
    use_chunk_op(resss.chunks, get_def_water_program(), resss.cam, 1);

    glUseProgram(get_def_skybox_program());
    use_skybox(resss.s, get_def_skybox_program());

    if (wireframe == 1)
    {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (ssao)
    {
      glUseProgram(get_def_ssao_program());
      use_lighting_ssao(resss.light, get_def_ssao_program());

      glUseProgram(get_def_ssao_blur_program());
      use_lighting_ssao_blur(resss.light, get_def_ssao_blur_program());
    }

    glUseProgram(get_def_deferred_br_program());
    use_lighting_deferred(resss.light, get_def_deferred_br_program());

    glUseProgram(get_def_post_process_program());
    use_lighting_postprocess(resss.light, get_def_post_process_program());

    glUseProgram(get_def_text_program());
    use_text_manager(resss.t, get_def_text_program());

    glfwSwapBuffers((GLFWwindow *)window);

    if (get_key_pressed(GLFW_KEY_F))
    {
      resss.light->fxaa = 1 - resss.light->fxaa;
    }

    run_jolt((float)get_frame_timems() / 1000.0f);
    end_game_loop();
  }

  delete_camera(resss.cam);
  delete_lighting(resss.light);

  delete_chunk_op(resss.chunks);

  delete_all_physic();
  delete_animations();
  delete_text_manager(resss.t);
  delete_skybox(resss.s);

  delete_body_jolt(resss.hm_boxes);
  delete_player(resss.p);
  deinit_jolt();
}

void loadmenu(void *window, unsigned char usetexture, float sealevel, int chunk_range, int chunk_size,
              int dimensionx, int dimensionz, int seedx, int seedz, unsigned char loadgsu, unsigned char ssao,
              unsigned char facemerged, unsigned char chunkanimations)
{
  int **hm = 0;
  if (usetexture)
  {
    hm = create_heightmap_texture("./heightmaps/test.jpeg", 200, 0, dimensionx, dimensionz);
    seedx = -1;
    seedz = -1;
  }
  else
  {
    DA *points = create_DA(sizeof(float));
    DA *heights = create_DA(sizeof(int));
    float tmp[] = {0, 0.30f, 0.34f, 0.37f, 0.41f, 0.44f, 0.46f, 0.48f, 0.49f, 0.51f, 0.52f, 0.54f, 0.57f, 0.60f, 0.64f, 0.67f, 0.71f, 1};
    pushback_many_DA(points, tmp, 18);
    int tmpi[] = {0, 35, 36, 47, 50, 50, 50, 52, 57, 64, 75, 85, 91, 93, 94, 94, 96, 100};
    pushback_many_DA(heights, tmpi, 18);

    hm = create_heightmap(dimensionx, dimensionz, seedx, seedz, 600, 0, 0, 0, 3, 2, 3, 0.3f, 30);

    delete_DA(points);
    delete_DA(heights);
  }

  gameloop(window, hm, seedx, seedz, dimensionx, dimensionz, sealevel, chunk_range,
           chunk_size, loadgsu, ssao, facemerged, chunkanimations);

  for (int i = 0; i < dimensionx; i++)
  {
    free(hm[i]);
  }
  free(hm);
}