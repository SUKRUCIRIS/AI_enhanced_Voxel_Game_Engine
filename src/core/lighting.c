#include "lighting.h"

lighting *create_lighting(void)
{
	lighting *l = malloc(sizeof(lighting));
	l->programs = create_DA(sizeof(GLuint));
	l->uniforms = create_DA(sizeof(GLint));
	return l;
}

void use_lighting(lighting *l, GLuint program)
{
	if (get_index_DA(l->programs, &program) == UINT_MAX)
	{
		pushback_DA(l->programs, &program);
		GLint uniform = glGetUniformLocation(program, "lightColor");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "lightDir");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "ambient");
		pushback_DA(l->uniforms, &uniform);
	}
	GLint *uniforms = get_data_DA(l->uniforms);
	glUniform4f(uniforms[get_index_DA(l->programs, &program) * 3], l->lightColor[0], l->lightColor[1], l->lightColor[2], l->lightColor[3]);
	glUniform3f(uniforms[get_index_DA(l->programs, &program) * 3 + 1], l->lightDir[0], l->lightDir[1], l->lightDir[2]);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 3 + 2], l->ambient);
}

void delete_lighting(lighting *l)
{
	delete_DA(l->programs);
	delete_DA(l->uniforms);
	free(l);
}