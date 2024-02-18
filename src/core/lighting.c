#include "lighting.h"
#include "float.h"
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

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
													float cascade1range, float cascade2range, float cascade3range, float fog_start, float fog_end,
													vec3 fog_color, unsigned char deferred)
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
	l->zMult = 10.0f;
	l->cam = cam;
	glm_normalize(l->lightDir);

	glfwGetWindowSize(window, &(l->windowwidth), &(l->windowheight));

	l->shadowMapWidth = shadowMapWidth;
	l->shadowMapHeight = shadowMapHeight;

	l->fog_start = fog_start;
	l->fog_end = fog_end;
	glm_vec3_copy(fog_color, l->fog_color);

	glGenFramebuffers(1, &l->shadowMapFBO);

	glGenTextures(1, &l->shadowMap);
	glBindTexture(GL_TEXTURE_2D_ARRAY, l->shadowMap);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, l->shadowMapWidth, l->shadowMapHeight, 4, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	float clampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, clampColor);

	glBindFramebuffer(GL_FRAMEBUFFER, l->shadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, l->shadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (deferred == 1)
	{
		glGenFramebuffers(1, &l->gbufferFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, l->gbufferFBO);

		glGenTextures(1, &l->gPosition);
		glBindTexture(GL_TEXTURE_2D, l->gPosition);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, l->windowwidth, l->windowheight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, l->gPosition, 0);

		glGenTextures(1, &l->gNormal);
		glBindTexture(GL_TEXTURE_2D, l->gNormal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, l->windowwidth, l->windowheight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, l->gNormal, 0);

		glGenTextures(1, &l->gTexCoord);
		glBindTexture(GL_TEXTURE_2D, l->gTexCoord);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, l->windowwidth, l->windowheight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, l->gTexCoord, 0);

		unsigned int attachments[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
		glDrawBuffers(3, attachments);

		glGenRenderbuffers(1, &l->gdepth);
		glBindRenderbuffer(GL_RENDERBUFFER, l->gdepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, l->windowwidth, l->windowheight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, l->gdepth);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		GLfloat quadVertices[] = {
				-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// top right
				-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // down right
				1.0f, 1.0f, 0.0f, 1.0f, 1.0f,		// top left
				1.0f, -1.0f, 0.0f, 1.0f, 0.0f,	// down left
		};
		GLuint quadindices[] = {2, 1, 0, 3, 1, 2};
		glGenVertexArrays(1, &(l->quadvao));
		glGenBuffers(1, &(l->quadvbo));
		glGenBuffers(1, &(l->quadebo));
		glBindVertexArray(l->quadvao);
		glBindBuffer(GL_ARRAY_BUFFER, l->quadvbo);
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), quadVertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, l->quadebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), quadindices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	return l;
}

void use_lighting(lighting *l, GLuint program, unsigned char shadowpass, unsigned char gpass, br_texture_manager *textures)
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
		uniform = glGetUniformLocation(program, "fog_start");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "fog_end");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "fog_color");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "gPosition");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "gNormal");
		pushback_DA(l->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "gTexCoord");
		pushback_DA(l->uniforms, &uniform);
	}
	GLint *uniforms = get_data_DA(l->uniforms);
	l->lightDir[2] = -l->lightDir[2];
	glUniform4f(uniforms[get_index_DA(l->programs, &program) * 16], l->lightColor[0], l->lightColor[1], l->lightColor[2], l->lightColor[3]);
	glUniform3f(uniforms[get_index_DA(l->programs, &program) * 16 + 1], l->lightDir[0], l->lightDir[1], l->lightDir[2]);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 2], l->ambient);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 3], l->specularStrength);
	glUniformMatrix4fv(uniforms[get_index_DA(l->programs, &program) * 16 + 4], 4, GL_FALSE, l->lightProjection[0][0]);
	glUniform1i(uniforms[get_index_DA(l->programs, &program) * 16 + 5], 31);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 6], l->cascade0range);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 7], l->cascade1range);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 8], l->cascade2range);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 9], l->cascade3range);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 10], l->fog_start);
	glUniform1f(uniforms[get_index_DA(l->programs, &program) * 16 + 11], l->fog_end);
	glUniform3f(uniforms[get_index_DA(l->programs, &program) * 16 + 12], l->fog_color[0], l->fog_color[1], l->fog_color[2]);
	glUniform1i(uniforms[get_index_DA(l->programs, &program) * 16 + 13], 30);
	glUniform1i(uniforms[get_index_DA(l->programs, &program) * 16 + 14], 29);
	glUniform1i(uniforms[get_index_DA(l->programs, &program) * 16 + 15], 28);
	l->lightDir[2] = -l->lightDir[2];
	if (shadowpass == 1)
	{
		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, l->shadowMapFBO);
		glViewport(0, 0, l->shadowMapWidth, l->shadowMapHeight);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	else if (gpass == 2)
	{
		glCullFace(GL_FRONT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, l->windowwidth, l->windowheight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D_ARRAY, l->shadowMap);
	}
	else if (gpass == 1)
	{
		glCullFace(GL_FRONT);
		glBindFramebuffer(GL_FRAMEBUFFER, l->gbufferFBO);
		glViewport(0, 0, l->windowwidth, l->windowheight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else if (gpass == 0)
	{
		glCullFace(GL_FRONT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, l->windowwidth, l->windowheight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glActiveTexture(GL_TEXTURE31);
		glBindTexture(GL_TEXTURE_2D_ARRAY, l->shadowMap);
		glActiveTexture(GL_TEXTURE30);
		glBindTexture(GL_TEXTURE_2D, l->gPosition);
		glActiveTexture(GL_TEXTURE29);
		glBindTexture(GL_TEXTURE_2D, l->gNormal);
		glActiveTexture(GL_TEXTURE28);
		glBindTexture(GL_TEXTURE_2D, l->gTexCoord);
		use_br_texture_manager(textures, program);
		glBindVertexArray(l->quadvao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void delete_lighting(lighting *l)
{
	glDeleteFramebuffers(1, &(l->shadowMapFBO));
	glDeleteTextures(1, &(l->shadowMap));
	glDeleteFramebuffers(1, &(l->gbufferFBO));
	glDeleteTextures(1, &(l->gPosition));
	glDeleteTextures(1, &(l->gNormal));
	glDeleteTextures(1, &(l->gTexCoord));
	glDeleteTextures(1, &(l->gdepth));
	glDeleteVertexArrays(1, &(l->quadvao));
	glDeleteBuffers(1, &(l->quadvbo));
	glDeleteBuffers(1, &(l->quadebo));
	delete_DA(l->programs);
	delete_DA(l->uniforms);
	free(l);
}