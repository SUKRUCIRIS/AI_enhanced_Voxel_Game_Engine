#include "player.h"

void run_input_player(player *p, GLFWwindow *window, long framems)
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
  glm_vec3_scale(move, p->speed * framems, move);
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