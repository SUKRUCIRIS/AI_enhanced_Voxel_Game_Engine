#include "player.h"
#include "core.h"

player *create_player(camera *fp_camera, float speed, float jumpspeed, float airspeedfactor,
                      float boostspeedfactor, float width, float height, int **hm, int dimensionx,
                      int dimensionz, const char *modelpath, float *start_pos, float maxslopeangle,
                      float maxstrength, float mass, unsigned char scaleall0_scaleonlyheight1)
{
  player *p = malloc(sizeof(player));
  p->speed = speed;
  p->jumpspeed = jumpspeed;
  p->airspeedfactor = airspeedfactor;
  p->boostspeedfactor = boostspeedfactor;
  p->jumping = 0;
  p->onland = 1;
  p->fp_camera = fp_camera;
  p->width = width;
  p->height = height;
  p->hm = hm;
  p->dimensionx = dimensionx;
  p->dimensionz = dimensionz;
  p->jumpdurationms = 100;
  p->model = create_br_object_manager();
  p->textures = create_br_texture_manager();
  p->phy = create_player_jolt(p->height, p->width / 2, maxslopeangle, maxstrength, mass, start_pos);
  {
    struct aiScene *player_model = load_model(modelpath, 1);
    br_scene player = load_object_br(p->model, p->textures, player_model, 0, 0, 3, 10, 0.1f, 0.5f);

    if (scaleall0_scaleonlyheight1 == 0)
    {
      float scalex = p->width / (player.box.mMax.x - player.box.mMin.x),
            scaley = p->height / (player.box.mMax.y - player.box.mMin.y),
            scalez = p->width / (player.box.mMax.z - player.box.mMin.z);

      vec3 scale = {scalex, scaley, scalez};

      for (unsigned int i2 = 0; i2 < player.mesh_count; i2++)
      {
        scale_br_object(player.meshes[i2], scale, 0);
        // scale in cpu beacuse i dont want the scale affect the translation
      }
    }
    else
    {
      float scaley = p->height / (player.box.mMax.y - player.box.mMin.y);

      vec3 scale = {scaley, scaley, scaley};

      for (unsigned int i2 = 0; i2 < player.mesh_count; i2++)
      {
        scale_br_object(player.meshes[i2], scale, 0);
        // scale in cpu beacuse i dont want the scale affect the translation
      }
    }

    prepare_render_br_object_manager(p->model);

    free_model(player_model);
    free(player.meshes);
    free(player.textures);
  }
  return p;
}

void delete_player(player *p)
{
  delete_player_jolt(p->phy);
  delete_br_object_manager(p->model);
  delete_br_texture_manager(p->textures);
  free(p);
}

void run_input_player(player *p, GLFWwindow *window, double framems, unsigned char fp0_tp1)
{
  run_input_fp_camera(p->fp_camera, window);
  vec3 move = {0, 0, 0};
  if (get_key_down(GLFW_KEY_W))
  {
    glm_vec3_add(move, p->fp_camera->orientation, move);
  }
  if (get_key_down(GLFW_KEY_S))
  {
    vec3 tmp;
    glm_vec3_scale(p->fp_camera->orientation, -1, tmp);
    glm_vec3_add(move, tmp, move);
  }
  if (get_key_down(GLFW_KEY_A))
  {
    vec3 cross;
    glm_vec3_cross(p->fp_camera->orientation, p->fp_camera->up, cross);
    glm_normalize_to(cross, cross);
    glm_vec3_scale(cross, -1, cross);
    glm_vec3_add(move, cross, move);
  }
  if (get_key_down(GLFW_KEY_D))
  {
    vec3 cross;
    glm_vec3_cross(p->fp_camera->orientation, p->fp_camera->up, cross);
    glm_normalize_to(cross, cross);
    glm_vec3_add(move, cross, move);
  }
  move[1] = 0;
  glm_normalize_to(move, move);
  vec3 moven;
  if (p->phy == 0)
  {
    move[1] = -1;
    if (p->jumping == 0 && p->onland == 1 && get_key_pressed(GLFW_KEY_SPACE))
    {
      p->jumping = 1;
      p->jumpstartms = get_timems();
    }
    else if (p->jumping == 1 && p->jumpstartms + p->jumpdurationms > get_timems())
    {
      move[1] = 4;
    }
    else if (p->jumping == 1 && p->jumpstartms + p->jumpdurationms <= get_timems())
    {
      p->jumping = 0;
    }
    glm_vec3_scale(move, p->speed * (float)framems, move);
    glm_vec3_add(move, p->fp_camera->position, p->fp_camera->position);
    int indexx = (int)(p->fp_camera->position[0] + p->dimensionx / 2.0f);
    int indexz = (int)(p->fp_camera->position[2] + p->dimensionz / 2.0f);
    if (indexx >= 0 && indexx < p->dimensionx && indexz >= 0 && indexz < p->dimensionz)
    {
      if (p->fp_camera->position[1] - p->height <= p->hm[indexx][indexz])
      {
        p->fp_camera->position[1] = p->hm[indexx][indexz] + p->height;
        p->onland = 1;
      }
      else
      {
        p->onland = 0;
      }
    }
  }
  else
  {
    glm_vec3_copy(move, moven);
    if (glm_vec3_norm2(move) == 0)
    {
      p->horizontalcontrol = 0;
    }
    else
    {
      p->horizontalcontrol = 1;
    }
    glm_vec3_scale(move, p->speed, move);
    if (!is_supported_jolt(p->phy))
    {
      glm_vec3_scale(move, p->airspeedfactor, move);
    }
    if (get_key_down(GLFW_KEY_LEFT_SHIFT))
    {
      glm_vec3_scale(move, p->boostspeedfactor, move);
    }
    if (get_key_pressed(GLFW_KEY_SPACE) && is_supported_jolt(p->phy))
    {
      p->jumping = 1;
      move[1] = p->jumpspeed;
    }
    else if (is_supported_jolt(p->phy) == 0)
    {
      p->jumping = 0;
      vec3 old;
      get_linear_velocity_player_jolt(p->phy, old);
      move[1] = old[1];
    }
    update_player_jolt(p->phy, (float)framems / 1000.0f, 0, 0, move, 1);

    get_position_player_jolt(p->phy, move);
    vec3 tmp;
    glm_vec3_copy(p->fp_camera->orientation, tmp);
    tmp[1] = 0;
    glm_normalize(tmp);
    if (fp0_tp1 == 0)
    {
      glm_vec3_scale(tmp, p->width / 2.0f, tmp);
      p->fp_camera->position[0] = move[0] + tmp[0];
      p->fp_camera->position[1] = move[1] + p->height - 0.5f;
      p->fp_camera->position[2] = move[2] + tmp[2];
    }
    else
    {
      glm_vec3_scale(tmp, -2, tmp);
      p->fp_camera->position[0] = move[0] + tmp[0];
      p->fp_camera->position[1] = move[1] + p->height;
      p->fp_camera->position[2] = move[2] + tmp[2];
    }
  }
  if (p->model)
  {
    get_position_player_jolt(p->phy, move);
    move[1] -= 0.03f;
    set_position_br_object_all(p->model, move);
    vec3 axis = {0, 1, 0};
    if (fp0_tp1 == 0)
    {
      set_rotation_br_object_all(p->model, glm_deg(-atan2f(p->fp_camera->orientation[2], p->fp_camera->orientation[0])), axis);
    }
    else
    {
      if (p->horizontalcontrol == 1)
      {
        set_rotation_br_object_all(p->model, glm_deg(-atan2f(moven[2], moven[0])), axis);
      }
    }
  }
}

void render_player(player *p, GLuint program)
{
  use_br_texture_manager(p->textures, program);
  use_br_object_manager(p->model, program);
}