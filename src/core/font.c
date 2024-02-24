#include "font.h"
#include "../../third_party/freetype/include/ft2build.h"
#include FT_FREETYPE_H

font *create_font(const char *font_file, int width, int height)
{
  FT_Library ft;
  FT_Init_FreeType(&ft);
  FT_Face face;
  FT_New_Face(ft, font_file, 0, &face);
  FT_Set_Pixel_Sizes(face, width, height);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

  font *f = malloc(sizeof(f));

  unsigned char *data[95];
  for (unsigned char c = 32; c < 127; c++)
  {
    FT_Load_Char(face, c, FT_LOAD_RENDER);
    data[c - 32] = malloc(sizeof(unsigned char) * face->glyph->bitmap.width * face->glyph->bitmap.rows);
    for (unsigned int i = 0; i < face->glyph->bitmap.width * face->glyph->bitmap.rows; i++)
    {
      data[c - 32][i] = face->glyph->bitmap.buffer[i];
    }
    f->sizex[c - 32] = face->glyph->bitmap.width;
    f->sizey[c - 32] = face->glyph->bitmap.rows;
    f->bearingx[c - 32] = face->glyph->bitmap_left;
    f->bearingy[c - 32] = face->glyph->bitmap_top;
    f->advance = face->glyph->advance.x;
  }
  glGenTextures(1, &f->font_textures);
  glBindTexture(GL_TEXTURE_2D_ARRAY, f->font_textures);
  glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RED, width, height, 95, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  float clampColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, clampColor);

  for (int i = 0; i < 95; i++)
  {
    free(data[i]);
  }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);
  return f;
}

void delete_font(font *f)
{
  glDeleteTextures(1, &(f->font_textures));
  delete_br_object_manager(f->text_quads);
  free(f);
}