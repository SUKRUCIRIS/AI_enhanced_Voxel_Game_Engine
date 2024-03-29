#include "font.h"
#include "../../third_party/freetype/include/ft2build.h"
#include <stdarg.h>
#include FT_FREETYPE_H
#ifdef _MSC_VER
#include <Windows.h>
#endif

text_manager *create_text_manager(const char *font_file, int height, int screenwidth, int screenheight,
                                  int realsw, int realsh, GLint min_filter, GLint mag_filter)
{
  FT_Library ft;
  if (FT_Init_FreeType(&ft))
  {
#ifdef _MSC_VER
    MessageBox(0, "FT_Init_FreeType Error", "Error", 0);
#endif
  }
  FT_Face face;
  if (FT_New_Face(ft, font_file, 0, &face))
  {
#ifdef _MSC_VER
    MessageBox(0, "FT_New_Face Error", "Error", 0);
#endif
  }
  if (FT_Set_Pixel_Sizes(face, 0, height))
  {
#ifdef _MSC_VER
    MessageBox(0, "FT_Set_Pixel_Sizes Error", "Error", 0);
#endif
  }

  text_manager *f = malloc(sizeof(text_manager));
  f->twidth = 0, f->theight = 0;
  for (unsigned char c = 32; c < 127; c++)
  {
    if (FT_Load_Char(face, c, FT_LOAD_BITMAP_METRICS_ONLY))
    {
#ifdef _MSC_VER
      MessageBox(0, "FT_Load_Char Error", "Error", 0);
#endif
    }

    f->sizex[c - 32] = face->glyph->bitmap.width;
    f->sizey[c - 32] = face->glyph->bitmap.rows;
    f->bearingx[c - 32] = face->glyph->bitmap_left;
    f->bearingy[c - 32] = face->glyph->bitmap_top;
    f->advancex[c - 32] = face->glyph->advance.x;
    f->twidth += face->glyph->bitmap.width;
    f->theight = max(f->theight, face->glyph->bitmap.rows);
  }

  f->twidth += 95; // extra 1 pixel between textures

  glGenTextures(1, &f->font_textures);
  glBindTexture(GL_TEXTURE_2D, f->font_textures);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, f->twidth, f->theight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  float clampColor[] = {1.0f, 1.0f, 1.0f, 0};
  glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, clampColor);

  int x = 0;

  for (unsigned char c = 32; c < 128; c++)
  {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    {
#ifdef _MSC_VER
      MessageBox(0, "FT_Load_Char Error", "Error", 0);
#endif
    }
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows,
                    GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    f->xoffset[c - 32] = (float)x / (float)f->twidth;
    x += face->glyph->bitmap.width + 1;
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  f->screenwidth = screenwidth;
  f->screenheight = screenheight;
  f->realsw = realsw;
  f->realsh = realsh;
  glm_ortho(0.0f, (float)screenwidth, 0.0f, (float)screenheight, -100.0f, 100.0f, f->projection);
  f->VAO = 0;
  f->VBO = 0;
  f->EBO = 0;
  f->vertices = create_DA_HIGH_MEMORY(sizeof(GLfloat));
  f->indices = create_DA_HIGH_MEMORY(sizeof(GLuint));
  f->programs = create_DA(sizeof(GLuint));
  f->uniforms = create_DA(sizeof(GLint));
  f->newdata = 0;
  return f;
}

void delete_text_manager(text_manager *f)
{
  glDeleteTextures(1, &(f->font_textures));
  delete_DA(f->vertices);
  delete_DA(f->indices);
  delete_DA(f->programs);
  delete_DA(f->uniforms);
  glDeleteVertexArrays(1, &(f->VAO));
  glDeleteBuffers(1, &(f->VBO));
  glDeleteBuffers(1, &(f->EBO));
  free(f);
}

void add_text(text_manager *f, float startx, float starty, float z, int scale, vec4 rgba, const char *text)
{
  startx = (float)((int)startx);
  starty = (float)((int)starty);
  f->newdata = 1;
  GLuint *indices = malloc(sizeof(GLuint) * 6 * strlen(text));
  unsigned int vertex_start = get_size_DA(f->vertices) / 9;
  for (unsigned int i = 0; i < strlen(text); i++)
  {
    indices[i * 6] = 2 + vertex_start + 4 * i;
    indices[i * 6 + 1] = 1 + vertex_start + 4 * i;
    indices[i * 6 + 2] = vertex_start + 4 * i;
    indices[i * 6 + 3] = vertex_start + 4 * i;
    indices[i * 6 + 4] = 3 + vertex_start + 4 * i;
    indices[i * 6 + 5] = 2 + vertex_start + 4 * i;
  }
  float old_startx = startx;
  for (unsigned int i = 0; i < strlen(text); i++)
  {
    if (text[i] == '\n')
    {
      starty -= (f->theight + 3) * scale;
      startx = old_startx;
      continue;
    }
    char ascii = text[i];
    if (ascii < 32 || ascii > 126)
    {
      ascii = 32;
    }
    ascii = ascii - 32;
    float xpos = startx + f->bearingx[ascii] * scale;
    float ypos = starty - (f->sizey[ascii] - f->bearingy[ascii]) * scale;
    float w = f->sizex[ascii] * (float)scale;
    float h = f->sizey[ascii] * (float)scale;
    startx += (f->advancex[ascii] >> 6) * scale;
    GLfloat vertices[] = {
        xpos, ypos + h, z, f->xoffset[ascii], 0, rgba[0], rgba[1], rgba[2], rgba[3],                                      // 0
        xpos, ypos, z, f->xoffset[ascii], (float)f->sizey[ascii] / (float)f->theight, rgba[0], rgba[1], rgba[2], rgba[3], // 1
        xpos + w, ypos, z, f->xoffset[ascii] + (float)f->sizex[ascii] / (float)f->twidth,
        (float)f->sizey[ascii] / (float)f->theight, rgba[0], rgba[1], rgba[2], rgba[3],                                              // 2
        xpos + w, ypos + h, z, f->xoffset[ascii] + (float)f->sizex[ascii] / (float)f->twidth, 0, rgba[0], rgba[1], rgba[2], rgba[3], // 3
    };
    pushback_many_DA(f->vertices, vertices, 36);
  }
  pushback_many_DA(f->indices, indices, 6 * (unsigned int)strlen(text));
  free(indices);
}

char static_text[1024];

void add_text_variadic(text_manager *f, float startx, float starty, float z, int scale, vec4 rgba, const char *text, ...)
{
  va_list args;
  va_start(args, text);
  vsnprintf(static_text, 1024 * sizeof(char), text, args);
  va_end(args);
  add_text(f, startx, starty, z, scale, rgba, static_text);
}

void get_text_size_variadic(text_manager *f, int scale, float *width, float *height, const char *text, ...)
{
  va_list args;
  va_start(args, text);
  vsnprintf(static_text, 1024 * sizeof(char), text, args);
  va_end(args);
  get_text_size(f, scale, static_text, width, height);
}

void get_text_size(text_manager *f, int scale, const char *text, float *width, float *height)
{
  *width = 0;
  *height = 0;
  float startx = 0;
  float starty = 0;
  float old_startx = startx;
  for (unsigned int i = 0; i < strlen(text); i++)
  {
    if (text[i] == '\n')
    {
      starty -= (f->theight + 3) * scale;
      startx = old_startx;
      continue;
    }
    char ascii = text[i];
    if (ascii < 32 || ascii > 126)
    {
      ascii = 32;
    }
    ascii = ascii - 32;
    float xpos = startx + f->bearingx[ascii] * scale;
    float ypos = starty - (f->sizey[ascii] - f->bearingy[ascii]) * scale;
    float w = f->sizex[ascii] * (float)scale;
    float h = f->sizey[ascii] * (float)scale;
    startx += (f->advancex[ascii] >> 6) * scale;
    *width = max(*width, xpos + w);
    *height = max(*height, ypos + h);
  }
}

void use_text_manager(text_manager *f, GLuint program)
{
  if ((f->VAO == 0 || f->newdata == 1) && get_size_DA(f->indices) > 0)
  {
    f->newdata = 0;
    glDeleteVertexArrays(1, &(f->VAO));
    glDeleteBuffers(1, &(f->VBO));
    glDeleteBuffers(1, &(f->EBO));
    glBindVertexArray(0);
    glGenVertexArrays(1, &(f->VAO));
    glGenBuffers(1, &(f->VBO));
    glGenBuffers(1, &(f->EBO));
    glBindVertexArray(f->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, f->VBO);
    glBufferData(GL_ARRAY_BUFFER, get_size_DA(f->vertices) * sizeof(GLfloat), get_data_DA(f->vertices), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, f->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, get_size_DA(f->indices) * sizeof(GLuint), get_data_DA(f->indices), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
  if (f->VAO != 0)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, f->realsw, f->realsh);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE31);
    glBindTexture(GL_TEXTURE_2D, f->font_textures);
    if (get_index_DA(f->programs, &program) == UINT_MAX)
    {
      pushback_DA(f->programs, &program);
      GLint uniform = glGetUniformLocation(program, "projection");
      pushback_DA(f->uniforms, &uniform);
      uniform = glGetUniformLocation(program, "font_textures");
      pushback_DA(f->uniforms, &uniform);
    }
    GLint *uniforms = get_data_DA(f->uniforms);
    glUniformMatrix4fv(uniforms[get_index_DA(f->programs, &program) * 2], 1, GL_FALSE, f->projection[0]);
    glUniform1i(uniforms[get_index_DA(f->programs, &program) * 2 + 1], 31);

    glBindVertexArray(f->VAO);
    glDrawElements(GL_TRIANGLES, get_size_DA(f->indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
  }
}

void clear_text_manager(text_manager *f)
{
  clear_DA(f->vertices);
  clear_DA(f->indices);
  glDeleteVertexArrays(1, &(f->VAO));
  glDeleteBuffers(1, &(f->VBO));
  glDeleteBuffers(1, &(f->EBO));
  f->VAO = 0;
  f->VBO = 0;
  f->EBO = 0;
}