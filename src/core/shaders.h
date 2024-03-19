#pragma once
#include "../../third_party/opengl/include/glad/glad.h"

void init_programs(void);

void destroy_programs(void);

GLuint get_def_program(void);

GLuint get_def_tex_program(void);

GLuint get_def_tex_light_program(void);

GLuint get_def_shadowmap_program(void);

GLuint get_def_tex_light_br_program(void);

GLuint get_def_tex_light_opt_br_program(void);

GLuint get_def_shadowmap_br_program(void);

GLuint get_def_tex_light_ins_program(void);

GLuint get_def_shadowmap_ins_program(void);

GLuint get_def_gbuffer_br_program(void);

GLuint get_def_deferred_br_program(void);

GLuint get_def_ssao_program(void);

GLuint get_def_ssao_blur_program(void);

GLuint get_def_post_process_program(void);

GLuint get_def_text_program(void);

GLuint get_def_skybox_program(void);

GLuint get_def_water_program(void);