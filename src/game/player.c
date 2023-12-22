#include "player.h"

void run_input_player(player *p, GLFWwindow *window, long framems)
{
  // run_input_fp_camera(p->fp_camera, window);
  run_input_free_camera(p->fp_camera, window);
}