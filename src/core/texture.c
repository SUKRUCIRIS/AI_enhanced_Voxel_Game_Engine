#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "../../third_party/stb/stb_image.h"
#include "texture.h"

texture *load_texture(const char *path, GLenum texType, GLenum pixelType, GLint min_filter, GLint mag_filter, float shininess)
{
	glBindTexture(texType, 0);
	texture *tex = calloc(1, sizeof(texture));
	int widthImg, heightImg, numColCh;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *bytes = stbi_load(path, &widthImg, &heightImg, &numColCh, 0);
	if (!bytes)
	{
		return 0;
	}
	GLenum format = GL_RGB;
	if (numColCh == 1)
	{
		format = GL_RED;
	}
	else if (numColCh == 3)
	{
		format = GL_RGB;
	}
	else if (numColCh == 4)
	{
		format = GL_RGBA;
	}
	glGenTextures(1, &(tex->id));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(texType, tex->id);
	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	glGenerateMipmap(texType);
	stbi_image_free(bytes);
	glBindTexture(texType, 0);
	tex->type = texType;
	tex->programs = create_DA(sizeof(GLuint));
	tex->uniforms = create_DA(sizeof(GLint));
	tex->shininess = shininess;
	return tex;
}

void use_texture(texture *tex, GLuint program)
{
	if (get_index_DA(tex->programs, &program) == UINT_MAX)
	{
		pushback_DA(tex->programs, &program);
		GLint uniform = glGetUniformLocation(program, "shininess");
		pushback_DA(tex->uniforms, &uniform);
		uniform = glGetUniformLocation(program, "tex0");
		pushback_DA(tex->uniforms, &uniform);
	}
	GLint *uniforms = get_data_DA(tex->uniforms);
	glUniform1f(uniforms[get_index_DA(tex->programs, &program) * 2], tex->shininess);
	glUniform1i(uniforms[get_index_DA(tex->programs, &program) * 2 + 1], 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(tex->type, tex->id);
}

void delete_texture(texture *tex)
{
	delete_DA(tex->programs);
	delete_DA(tex->uniforms);
	glDeleteTextures(1, &(tex->id));
	free(tex);
}