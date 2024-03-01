#version 400 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gTexCoord;

in vec3 texCoords;

uniform samplerCube cubemap;

void main()
{
  gTexCoord=vec4(0,texture(cubemap, texCoords).xyz);
}