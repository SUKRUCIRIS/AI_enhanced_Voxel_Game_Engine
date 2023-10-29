#include "lighting.h"

lighting *create_lighting(GLFWwindow *window)
{
	lighting *l = malloc(sizeof(lighting));
	l->programs = create_DA(sizeof(GLuint));
	l->uniforms = create_DA(sizeof(GLint));

	l->ambient = 0.5f;
	l->lightColor[0] = 0.5294f;
	l->lightColor[1] = 0.8078f;
	l->lightColor[2] = 0.9216f;
	l->lightColor[3] = 1;
	l->lightDir[0] = 0.5f;
	l->lightDir[1] = 0.5f;
	l->lightDir[2] = 0.5f;
	l->specularStrength = 0.5f;

	l->center[0] = 0;
	l->center[1] = 0;
	l->center[2] = 0;
	l->up[0] = 0;
	l->up[1] = 1;
	l->up[2] = 0;

	glfwGetWindowSize(window, &(l->windowwidth), &(l->windowheight));

	l->shadowMapWidth = 4096;
	l->shadowMapHeight = 4096;

	glGenFramebuffers(1, &l->shadowMapFBO);

	glGenTextures(1, &l->shadowMap);
	glBindTexture(GL_TEXTURE_2D, l->shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, l->shadowMapWidth, l->shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float clampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, l->shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, l->shadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glm_ortho(-10, 10, -10, 10, 0.1f, 10.0f, l->orthgonalProjection);
	glm_lookat(l->lightDir, l->center, l->up, l->lightView);
	glm_mat4_mul(l->orthgonalProjection, l->lightView, l->lightProjection);

	return l;
}

void use_lighting(lighting *l, GLuint program, unsigned char shadowpass)
{

	glm_lookat(l->lightDir, l->center, l->up, l->lightView);
	glm_mat4_mul(l->orthgonalProjection, l->lightView, l->lightProjection);

	if (get_index_DA(l->programs, &program) == UINT_MAX)
	{
		pushback_DA(l->programs, &program);
		GLint uniform = glGetUniformLocation(program, "lightColor");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "lightDir");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "ambient");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "specularStrength");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "lightProjection");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "shadowMap");
		pushback_DA(l->uniforms, &uniform);
	}
	GLint *uniforms = get_data_DA(l->uniforms);
	glUniform4f(uniforms[get_index_DA(l->programs, &program) * 6], l->lightColor[0], l->lightColor[1], l->lightColor[2], l->lightColor[3]);
	glUniform3f(uniforms[get_index_DA(l->programs, &program) * 6 + 1], l->lightDir[0], l->lightDir[1], l->lightDir[2]);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 6 + 2], l->ambient);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 6 + 3], l->specularStrength);
	glUniformMatrix4fv(uniforms[get_index_DA(l->programs, &program) * 6 + 4], 1, GL_FALSE, l->lightProjection[0]);
	glUniform1i(uniforms[get_index_DA(l->programs, &program) * 6 + 5], 1);
	if (shadowpass)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, l->shadowMapFBO);
		glViewport(0, 0, l->shadowMapWidth, l->shadowMapHeight);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, l->windowwidth, l->windowheight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, l->shadowMap);
	}
}

void delete_lighting(lighting *l)
{
	glDeleteFramebuffers(1, &(l->shadowMapFBO));
	glDeleteTextures(1, &(l->shadowMap));
	delete_DA(l->programs);
	delete_DA(l->uniforms);
	free(l);
}