#include "player.h"

void run_input_fp_player(player *p, GLFWwindow *window, double framems)
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
      move[1] = p->jumpspeed;
    }
    else
    {
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
    glm_vec3_scale(tmp, p->width / 2.0f, tmp);
    p->fp_camera->position[0] = move[0] + tmp[0];
    p->fp_camera->position[1] = move[1] + p->height - 0.5f;
    p->fp_camera->position[2] = move[2] + tmp[2];
  }
  if (p->model)
  {
    get_position_player_jolt(p->phy, move);
    move[1] -= 0.055f;
    set_position_br_object_all(p->model, move);

    vec3 axis = {0, 1, 0};
    set_rotation_br_object_all(p->model, p->fp_camera->uprot, axis);
  }
}