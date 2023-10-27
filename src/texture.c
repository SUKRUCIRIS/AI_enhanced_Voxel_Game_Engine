#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb/stb_image.h"
#include "../third_party/opengl/include/glad/glad.h"
#include "texture.h"

struct texture
{
	GLuint id;
	GLenum type;
};

texture *load_texture(const char *path, GLenum texType, GLenum pixelType, GLint min_filter, GLint mag_filter)
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
	return tex;
}

void use_texture(texture *tex)
{
	glBindTexture(tex->type, tex->id);
}

void delete_texture(texture *tex)
{
	glDeleteTextures(1, &(tex->id));
	free(tex);
}