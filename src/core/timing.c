#include "timing.h"
#include "../../third_party/glfw/include/GLFW/glfw3.h"

double timer = 0;
double frame_ms = 0;

void start_game_loop(void)
{
  timer = glfwGetTime() * 1000;
}

void end_game_loop(void)
{
  frame_ms = glfwGetTime() * 1000 - timer;
}

void end_game_loop_targetms(double targetms)
{
  while (frame_ms < targetms)
  {
    frame_ms = glfwGetTime() * 1000 - timer;
  }
}

double get_frame_timems(void)
{
  return frame_ms;
}

double get_timems(void)
{
  return glfwGetTime() * 1000;
}