#include "shaders.h"
#include <stdio.h>
#include <stdlib.h>

GLuint def_program = 0;
GLuint def_tex_program = 0;
GLuint def_tex_light_program = 0;
GLuint def_shadowmap_program = 0;
GLuint def_tex_light_br_program = 0;
GLuint def_tex_light_opt_br_program = 0;
GLuint def_shadowmap_br_program = 0;
GLuint def_tex_light_ins_program = 0;
GLuint def_shadowmap_ins_program = 0;

char *get_shader_content(const char *fileName)
{
	FILE *fp;
	long size = 0;
	char *shaderContent = 0;
	fp = fopen(fileName, "rb");
	if (fp == 0)
	{
		return 0;
	}
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp) + 1;
	fclose(fp);
	fp = fopen(fileName, "r");
	shaderContent = calloc(size, 1);
	fread(shaderContent, 1, size - 1, fp);
	fclose(fp);
	return shaderContent;
}

GLuint compile_program(const char *frag_shader_file, const char *vert_shader_file, const char *geo_shader_file)
{
	char *vertexShaderSource = get_shader_content(vert_shader_file);
	char *fragmentShaderSource = get_shader_content(frag_shader_file);
	char *geoshadersource = 0;
	if (geo_shader_file != 0)
	{
		geoshadersource = get_shader_content(geo_shader_file);
	}
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char *const *)&vertexShaderSource, 0);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char *const *)&fragmentShaderSource, 0);
	glCompileShader(fragmentShader);
	GLuint geoshader = 0;
	if (geo_shader_file != 0)
	{
		geoshader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geoshader, 1, (const char *const *)&geoshadersource, 0);
		glCompileShader(geoshader);
	}
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	if (geo_shader_file != 0)
	{
		glAttachShader(shaderProgram, geoshader);
	}
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	if (geo_shader_file != 0)
	{
		glDeleteShader(geoshader);
	}
	free(vertexShaderSource);
	free(fragmentShaderSource);
	free(geoshadersource);
	return shaderProgram;
}

void init_programs(void)
{
	def_program = compile_program("./shaders/def.fs", "./shaders/def.vs", 0);
	def_tex_program = compile_program("./shaders/def_tex.fs", "./shaders/def_tex.vs", 0);
	def_tex_light_program = compile_program("./shaders/def_tex_light.fs", "./shaders/def_tex_light.vs", 0);
	def_shadowmap_program = compile_program("./shaders/def_shadowmap.fs", "./shaders/def_shadowmap.vs", "./shaders/def_shadowmap.gs");
	def_tex_light_br_program = compile_program("./shaders/def_tex_light_br.fs", "./shaders/def_tex_light_br.vs", 0);
	def_tex_light_opt_br_program = compile_program("./shaders/def_tex_light_opt_br.fs", "./shaders/def_tex_light_opt_br.vs", 0);
	def_shadowmap_br_program = compile_program("./shaders/def_shadowmap.fs", "./shaders/def_shadowmap_br.vs", "./shaders/def_shadowmap.gs");
	def_tex_light_ins_program = compile_program("./shaders/def_tex_light_br.fs", "./shaders/def_tex_light_ins.vs", 0);
	def_shadowmap_ins_program = compile_program("./shaders/def_shadowmap.fs", "./shaders/def_shadowmap_ins.vs", "./shaders/def_shadowmap.gs");
}

void destroy_programs(void)
{
	glDeleteProgram(def_program);
	glDeleteProgram(def_tex_program);
	glDeleteProgram(def_tex_light_program);
	glDeleteProgram(def_shadowmap_program);
	glDeleteProgram(def_tex_light_br_program);
	glDeleteProgram(def_tex_light_opt_br_program);
	glDeleteProgram(def_shadowmap_br_program);
	glDeleteProgram(def_tex_light_ins_program);
	glDeleteProgram(def_shadowmap_ins_program);
}

GLuint get_def_program(void)
{
	return def_program;
}

GLuint get_def_tex_program(void)
{
	return def_tex_program;
}

GLuint get_def_tex_light_program(void)
{
	return def_tex_light_program;
}

GLuint get_def_shadowmap_program(void)
{
	return def_shadowmap_program;
}

GLuint get_def_tex_light_br_program(void)
{
	return def_tex_light_br_program;
}

GLuint get_def_tex_light_opt_br_program(void)
{
	return def_tex_light_opt_br_program;
}

GLuint get_def_shadowmap_br_program(void)
{
	return def_shadowmap_br_program;
}

GLuint get_def_tex_light_ins_program(void)
{
	return def_tex_light_ins_program;
}

GLuint get_def_shadowmap_ins_program(void)
{
	return def_shadowmap_ins_program;
}