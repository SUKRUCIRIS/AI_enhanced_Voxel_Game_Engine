#version 400 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec4 rgba;

out vec2 TexCoords;
out vec4 RGBA;

uniform mat4 projection;

void main()
{
  TexCoords = tex;
  RGBA = rgba;
  gl_Position = projection * vec4(pos, 1.0);
}