#include "./core/core.h"
#include "./game/gameloop.h"

int main(void)
{
	GLFWwindow *window = create_window(0, 0, 1, 1, 0);
	if (window == 0)
	{
		return -1;
	}
	random_seed(get_timems() * get_timems());
	init_programs();

	unsigned char usetexture = 1;
	float sealevel = 5.3f;
	int chunk_size = 16;
	int chunk_range = 16;
	int dimensionx = 2048;
	int dimensionz = 2048;
	int seedx = 1453;
	int seedz = 1071;

	loadmenu(window, usetexture, sealevel, chunk_range, chunk_size, dimensionx, dimensionz, seedx, seedz);

	destroy_programs();
	delete_window(window);
	return 0;
}