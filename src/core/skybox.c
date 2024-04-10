#include "skybox.h"
#include "stdlib.h"
#include "../../third_party/stb/stb_image.h"
#include "timing.h"

float skyboxVertices[] =
    {
        //   Coordinates
        -1.0f, -1.0f, 1.0f,  //        7--------6
        1.0f, -1.0f, 1.0f,   //       /|       /|
        1.0f, -1.0f, -1.0f,  //      4--------5 |
        -1.0f, -1.0f, -1.0f, //      | |      | |
        -1.0f, 1.0f, 1.0f,   //      | 3------|-2
        1.0f, 1.0f, 1.0f,    //      |/       |/
        1.0f, 1.0f, -1.0f,   //      0--------1
        -1.0f, 1.0f, -1.0f};

unsigned int skyboxIndices[] =
    {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3};

skybox *create_skybox(const char *right_texture, const char *left_texture, const char *top_texture,
                      const char *bottom_texture, const char *front_texture, const char *back_texture,
                      camera *cam, float rotate_frame_ms, vec3 rotate_axis)
{
  skybox *s = malloc(sizeof(skybox));
  glGenVertexArrays(1, &s->VAO);
  glGenBuffers(1, &s->VBO);
  glGenBuffers(1, &s->EBO);
  glBindVertexArray(s->VAO);
  glBindBuffer(GL_ARRAY_BUFFER, s->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  s->programs = create_DA(sizeof(GLuint));
  s->uniforms = create_DA(sizeof(GLint));
  s->cam = cam;
  glGenTextures(1, &s->cubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, s->cubemap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  // These are very important to prevent seams
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // This might help with seams on some systems
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  int width, height, nrChannels;
  {
    unsigned char *data = stbi_load(right_texture, &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    GLenum format = GL_RGB;
    if (nrChannels == 1)
    {
      format = GL_RED;
    }
    else if (nrChannels == 3)
    {
      format = GL_RGB;
    }
    else if (nrChannels == 4)
    {
      format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X,
        0,
        GL_RGBA,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data);
    stbi_image_free(data);
  }
  {
    unsigned char *data = stbi_load(left_texture, &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    GLenum format = GL_RGB;
    if (nrChannels == 1)
    {
      format = GL_RED;
    }
    else if (nrChannels == 3)
    {
      format = GL_RGB;
    }
    else if (nrChannels == 4)
    {
      format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + 1,
        0,
        GL_RGBA,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data);
    stbi_image_free(data);
  }
  {
    unsigned char *data = stbi_load(top_texture, &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    GLenum format = GL_RGB;
    if (nrChannels == 1)
    {
      format = GL_RED;
    }
    else if (nrChannels == 3)
    {
      format = GL_RGB;
    }
    else if (nrChannels == 4)
    {
      format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + 2,
        0,
        GL_RGBA,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data);
    stbi_image_free(data);
  }
  {
    unsigned char *data = stbi_load(bottom_texture, &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    GLenum format = GL_RGB;
    if (nrChannels == 1)
    {
      format = GL_RED;
    }
    else if (nrChannels == 3)
    {
      format = GL_RGB;
    }
    else if (nrChannels == 4)
    {
      format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + 3,
        0,
        GL_RGBA,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data);
    stbi_image_free(data);
  }
  {
    unsigned char *data = stbi_load(front_texture, &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    GLenum format = GL_RGB;
    if (nrChannels == 1)
    {
      format = GL_RED;
    }
    else if (nrChannels == 3)
    {
      format = GL_RGB;
    }
    else if (nrChannels == 4)
    {
      format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + 4,
        0,
        GL_RGBA,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data);
    stbi_image_free(data);
  }
  {
    unsigned char *data = stbi_load(back_texture, &width, &height, &nrChannels, 0);
    stbi_set_flip_vertically_on_load(false);
    GLenum format = GL_RGB;
    if (nrChannels == 1)
    {
      format = GL_RED;
    }
    else if (nrChannels == 3)
    {
      format = GL_RGB;
    }
    else if (nrChannels == 4)
    {
      format = GL_RGBA;
    }
    glTexImage2D(
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + 5,
        0,
        GL_RGBA,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data);
    stbi_image_free(data);
  }
  glm_vec3_copy(rotate_axis, s->rotate_axis);
  glm_normalize(s->rotate_axis);
  s->rotate_frame = rotate_frame_ms;
  s->rotate_full = 0;
  return s;
}

void delete_skybox(skybox *s)
{
  glDeleteVertexArrays(1, &(s->VAO));
  glDeleteBuffers(1, &(s->VBO));
  glDeleteBuffers(1, &(s->EBO));
  glDeleteTextures(1, &(s->cubemap));
  delete_DA(s->programs);
  delete_DA(s->uniforms);
  free(s);
}

void use_skybox(skybox *s, GLuint program)
{
  mat3 x;
  calculate_camera(s->cam, s->cam->nearPlane, s->cam->farPlane);
  glm_mat4_pick3(s->cam->view, x);
  glm_mat4_identity(s->result);
  glm_mat4_ins3(x, s->result);
  glm_mat4_mul(s->cam->projection, s->result, s->result);
  s->rotate_full += s->rotate_frame * (float)get_frame_timems();
  s->rotate_full = fmodf(s->rotate_full, 360.0f);
  glm_rotate(s->result, s->rotate_full, s->rotate_axis);
  if (get_index_DA(s->programs, &program) == UINT_MAX)
  {
    pushback_DA(s->programs, &program);
    GLint uniform = glGetUniformLocation(program, "cam");
    pushback_DA(s->uniforms, &uniform);
    uniform = glGetUniformLocation(program, "cubemap");
    pushback_DA(s->uniforms, &uniform);
  }
  GLint *uniforms = get_data_DA(s->uniforms);
  unsigned int index = get_index_DA(s->programs, &program);
  glUniformMatrix4fv(uniforms[index * 2], 1, GL_FALSE, s->result[0]);
  glUniform1i(uniforms[index * 2 + 1], 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, s->cubemap);
  glBindVertexArray(s->VAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}