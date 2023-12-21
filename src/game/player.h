#pragma once
#include "../core/core.h"

typedef struct player
{
  camera *fp_camera;
} player;

void run_input_player(player *p, GLFWwindow *window, long framems);
