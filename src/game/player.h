#pragma once
#include "../core/core.h"

typedef struct player
{
  camera *fp_camera;
  float speed;
  vec3 minaabb;
  vec3 maxaabb;
  unsigned char jumping;
} player;

void run_input_player(player *p, GLFWwindow *window, long framems);
