#include "shaders.h"
#include <stdio.h>
#include <stdlib.h>

GLuint def_program = 0;
GLuint def_tex_program = 0;

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

GLuint compile_program(const char *frag_shader_file, const char *vert_shader_file)
{
	char *vertexShaderSource = get_shader_content(vert_shader_file);
	char *fragmentShaderSource = get_shader_content(frag_shader_file);
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char *const *)&vertexShaderSource, 0);
	glCompileShader(vertexShader);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char *const *)&fragmentShaderSource, 0);
	glCompileShader(fragmentShader);
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	free(vertexShaderSource);
	free(fragmentShaderSource);
	return shaderProgram;
}

void init_programs(void)
{
	def_program = compile_program("../shaders/def.fs", "../shaders/def.vs");
	def_tex_program = compile_program("../shaders/def_tex.fs", "../shaders/def_tex.vs");
}

void destroy_programs(void)
{
	glDeleteProgram(def_program);
}

GLuint get_def_program(void)
{
	return def_program;
}

GLuint get_def_tex_program(void)
{
	return def_tex_program;
}