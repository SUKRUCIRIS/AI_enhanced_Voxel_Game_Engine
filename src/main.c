#include "./core/core.h"
#include "./game/gameloop.h"

int main(void)
{
	GLFWwindow *window = create_window(0, 0, 1, 1, 0);
	if (window == 0)
	{
		return -1;
	}
	random_seed(get_timems());
	init_programs();

	unsigned char usetexture = 1;

	int **hm = 0;
	int seedx = rand();
	int seedz = rand();
	int dimensionx = 2048, dimensionz = 2048;
	if (usetexture)
	{
		hm = create_heightmap_texture("./heightmaps/test.jpeg", 200, 0, dimensionx, dimensionz);
		seedx = -1;
		seedz = -1;
	}
	else
	{
		DA *points = create_DA(sizeof(float));
		DA *heights = create_DA(sizeof(int));
		float tmp[] = {0, 0.30f, 0.34f, 0.37f, 0.41f, 0.44f, 0.46f, 0.48f, 0.49f, 0.51f, 0.52f, 0.54f, 0.57f, 0.60f, 0.64f, 0.67f, 0.71f, 1};
		pushback_many_DA(points, tmp, 18);
		int tmpi[] = {0, 35, 36, 47, 50, 50, 50, 52, 57, 64, 75, 85, 91, 93, 94, 94, 96, 100};
		pushback_many_DA(heights, tmpi, 18);

		hm = create_heightmap(dimensionx, dimensionz, seedx, seedz, 600, 0, 0, 0, 3, 2, 3, 0.3f, 30);

		delete_DA(points);
		delete_DA(heights);
	}

	gameloop(window, hm, seedx, seedz, dimensionx, dimensionz);

	for (int i = 0; i < dimensionx; i++)
	{
		free(hm[i]);
	}
	free(hm);
	destroy_programs();
	delete_window(window);
	return 0;
}