#pragma once
#include "../../third_party/opengl/include/glad/glad.h"

void init_programs(void);

void destroy_programs(void);

GLuint get_def_program(void);

GLuint get_def_tex_program(void);

GLuint get_def_tex_light_program(void);

GLuint get_def_shadowmap_program(void);