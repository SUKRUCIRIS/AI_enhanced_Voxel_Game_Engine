#version 400 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gTexCoord;

in vec3 texCoords;

uniform samplerCube cubemap;

void main()
{
  gTexCoord=texture(cubemap, texCoords).xyz;
}