#version 400 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gTexCoord;
layout (location = 3) out vec3 gTexCoordcopy;

in vec2 texCoord;
in vec3 normal;
in vec3 crntPos;
flat in float texture_id;

uniform sampler2D textures[32];

void main(){
  gPosition=crntPos;
  gNormal=normal;
  gTexCoord=vec3(texture(textures[int(texture_id)], texCoord));
  gTexCoordcopy=gTexCoord;
}