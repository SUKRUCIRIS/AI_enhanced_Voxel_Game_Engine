#include "timing.h"
#include "../../third_party/glfw/include/GLFW/glfw3.h"

double timer = 0;
double frame_ms = 0;
double old_frame_ms = 0;

unsigned long long int avg_count = 0;
double average_ms = 0;

void start_game_loop(void)
{
  timer = glfwGetTime() * 1000;
  old_frame_ms = frame_ms;
  frame_ms = 0;
}

void end_game_loop(void)
{
  frame_ms = glfwGetTime() * 1000 - timer;
  average_ms = (average_ms * avg_count + frame_ms) / (avg_count + 1);
  avg_count++;
}

void end_game_loop_targetms(double targetms)
{
  while (frame_ms < targetms)
  {
    frame_ms = glfwGetTime() * 1000 - timer;
  }
  average_ms = (average_ms * avg_count + frame_ms) / (avg_count + 1);
  avg_count++;
}

double get_frame_timems(void)
{
  return old_frame_ms;
}

double get_timems(void)
{
  return glfwGetTime() * 1000;
}

double get_average_frame_timems(void)
{
  return average_ms;
}