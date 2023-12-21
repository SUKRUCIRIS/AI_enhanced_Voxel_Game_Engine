#include "timing.h"
#include <time.h>

clock_t timer = 0;
clock_t frame_ms = 0;

void start_game_loop(void)
{
  timer = clock();
}

void end_game_loop(void)
{
  frame_ms = (clock() - timer) / (CLOCKS_PER_SEC / 1000);
}

void end_game_loop_targetms(long targetms)
{
  while (frame_ms < targetms)
  {
    frame_ms = (clock() - timer) / (CLOCKS_PER_SEC / 1000);
  }
}

long get_frame_timems(void)
{
  return frame_ms;
}