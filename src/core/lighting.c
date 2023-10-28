#include "lighting.h"

lighting *create_lighting(void)
{
	lighting *l = malloc(sizeof(lighting));
	l->programs = create_DA(sizeof(GLuint));
	l->uniforms = create_DA(sizeof(GLint));
	return l;
}

void use_lighting(lighting *l, GLuint program, camera *cam)
{
	if (get_index_DA(l->programs, &program) == UINT_MAX)
	{
		pushback_DA(l->programs, &program);
		GLint uniform = glGetUniformLocation(program, "lightColor");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "lightPos");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "camPos");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "ambient");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "specularLight");
		pushback_DA(l->uniforms, &uniform);
	}
	GLint *uniforms = get_data_DA(l->uniforms);
	glUniform4f(uniforms[get_index_DA(l->programs, &program) * 5], l->lightColor[0], l->lightColor[1], l->lightColor[2], l->lightColor[3]);
	glUniform3f(uniforms[get_index_DA(l->programs, &program) * 5 + 1], l->lightPos[0], l->lightPos[1], l->lightPos[2]);
	glUniform3f(uniforms[get_index_DA(l->programs, &program) * 5 + 2], cam->position[0], cam->position[1], cam->position[2]);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 5 + 3], l->ambient);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 5 + 4], l->specularLight);
}

void delete_lighting(lighting *l)
{
	delete_DA(l->programs);
	delete_DA(l->uniforms);
	free(l);
}