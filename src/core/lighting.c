#include "lighting.h"
#include "float.h"

void calculate_lighting_projection(lighting *l, int step)
{
	if (step == 0)
	{
		calculate_camera(l->cam, l->cam->nearPlane, l->cascade0range);
	}
	else if (step == 1)
	{
		calculate_camera(l->cam, l->cascade0range, l->cascade1range);
	}
	else if (step == 2)
	{
		calculate_camera(l->cam, l->cascade1range, l->cascade2range);
	}
	else if (step == 3)
	{
		calculate_camera(l->cam, l->cascade2range, l->cascade3range);
	}
	glm_mat4_inv(l->cam->result, l->camresultinv);
	glm_frustum_corners(l->camresultinv, l->frustumcorners);
	glm_frustum_center(l->frustumcorners, l->frustumcenter);
	glm_vec3_add(l->frustumcenter, l->lightDir, l->lookatinput1);
	glm_lookat(l->lookatinput1, l->frustumcenter, l->up, l->lightView);

	l->minX = FLT_MAX;
	l->maxX = -FLT_MAX;
	l->minY = FLT_MAX;
	l->maxY = -FLT_MAX;
	l->minZ = FLT_MAX;
	l->maxZ = -FLT_MAX;

	for (int i = 0; i < 8; i++)
	{
		glm_mat4_mulv(l->lightView, l->frustumcorners[i], l->tmp);
		l->minX = min(l->minX, l->tmp[0]);
		l->maxX = max(l->maxX, l->tmp[0]);
		l->minY = min(l->minY, l->tmp[1]);
		l->maxY = max(l->maxY, l->tmp[1]);
		l->minZ = min(l->minZ, l->tmp[2]);
		l->maxZ = max(l->maxZ, l->tmp[2]);
	}
	if (l->minZ < 0)
	{
		l->minZ *= l->zMult;
	}
	else
	{
		l->minZ /= l->zMult;
	}
	if (l->maxZ < 0)
	{
		l->maxZ /= l->zMult;
	}
	else
	{
		l->maxZ *= l->zMult;
	}
	glm_ortho(l->minX, l->maxX, l->minY, l->maxY, l->minZ, l->maxZ, l->orthgonalProjection);
	glm_mat4_mul(l->orthgonalProjection, l->lightView, l->lightProjection[step]);
}

lighting *create_lighting(GLFWwindow *window, camera *cam, GLuint shadowMapWidth, GLuint shadowMapHeight, float cascade0range,
						  float cascade1range, float cascade2range, float cascade3range)
{
	lighting *l = malloc(sizeof(lighting));
	l->programs = create_DA(sizeof(GLuint));
	l->uniforms = create_DA(sizeof(GLint));

	l->cascade0range = cascade0range;
	l->cascade1range = cascade1range;
	l->cascade2range = cascade2range;
	l->cascade3range = cascade3range;

	l->ambient = 0.5f;
	l->lightColor[0] = 0.5294f;
	l->lightColor[1] = 0.8078f;
	l->lightColor[2] = 0.9216f;
	l->lightColor[3] = 1;
	l->lightDir[0] = 20;
	l->lightDir[1] = 50;
	l->lightDir[2] = 20;
	l->specularStrength = 0.5f;

	l->center[0] = 0;
	l->center[1] = 0;
	l->center[2] = 0;
	l->up[0] = 0;
	l->up[1] = 1;
	l->up[2] = 0;
	l->zMult = 4.0f;
	l->cam = cam;
	glm_normalize(l->lightDir);

	glfwGetWindowSize(window, &(l->windowwidth), &(l->windowheight));

	l->shadowMapWidth = shadowMapWidth;
	l->shadowMapHeight = shadowMapHeight;

	glGenFramebuffers(1, &l->shadowMapFBO);

	glGenTextures(1, &l->shadowMap);
	glBindTexture(GL_TEXTURE_2D_ARRAY, l->shadowMap);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, l->shadowMapWidth, l->shadowMapHeight, 4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float clampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, l->shadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, l->shadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return l;
}

void use_lighting(lighting *l, GLuint program, unsigned char shadowpass)
{
	if (shadowpass)
	{
		calculate_lighting_projection(l, 0);
		calculate_lighting_projection(l, 1);
		calculate_lighting_projection(l, 2);
		calculate_lighting_projection(l, 3);
	}

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
		uniform = glGetUniformLocation(program, "cascade0range");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "cascade1range");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "cascade2range");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "cascade3range");
		pushback_DA(l->uniforms, &uniform);
	}
	GLint *uniforms = get_data_DA(l->uniforms);
	l->lightDir[2] = -l->lightDir[2];
	glUniform4f(uniforms[get_index_DA(l->programs, &program) * 10], l->lightColor[0], l->lightColor[1], l->lightColor[2], l->lightColor[3]);
	glUniform3f(uniforms[get_index_DA(l->programs, &program) * 10 + 1], l->lightDir[0], l->lightDir[1], l->lightDir[2]);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 10 + 2], l->ambient);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 10 + 3], l->specularStrength);
	glUniformMatrix4fv(uniforms[get_index_DA(l->programs, &program) * 10 + 4], 4, GL_FALSE, l->lightProjection[0][0]);
	glUniform1i(uniforms[get_index_DA(l->programs, &program) * 10 + 5], 1);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 10 + 6], l->cascade0range);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 10 + 7], l->cascade1range);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 10 + 8], l->cascade2range);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 10 + 9], l->cascade3range);
	l->lightDir[2] = -l->lightDir[2];
	if (shadowpass)
	{
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, l->shadowMapFBO);
		glViewport(0, 0, l->shadowMapWidth, l->shadowMapHeight);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else
	{
		glCullFace(GL_FRONT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, l->windowwidth, l->windowheight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D_ARRAY, l->shadowMap);
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