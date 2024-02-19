#include "random.h"
#include <stdlib.h>

void random_seed(double time_ms)
{
  srand((unsigned int)time_ms);
}

int random_int(int min, int max)
{
  return rand() % (max - min + 1) + min;
}

float random_float(float min, float max)
{
  return min + ((float)rand() / RAND_MAX) * (max - min);
}