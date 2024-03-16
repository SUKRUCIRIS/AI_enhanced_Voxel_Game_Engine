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

	gameloop(window);

	destroy_programs();
	delete_window(window);
	return 0;
}