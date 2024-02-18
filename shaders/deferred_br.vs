#version 400 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoords;

out vec2 TexCoords;

void main()
{
  TexCoords = texcoords;
  gl_Position = vec4(pos, 1.0);
}