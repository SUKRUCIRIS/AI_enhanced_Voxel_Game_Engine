#include "player.h"

void run_input_player(player *p, GLFWwindow *window, long framems)
{
  run_input_fp_camera(p->fp_camera, window);
  vec3 velocity = {0};
  if (get_key_down(GLFW_KEY_W))
  {
  }
}