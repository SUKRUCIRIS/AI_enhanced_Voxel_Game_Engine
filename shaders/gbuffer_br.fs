#version 400 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gTexCoord;
layout (location = 3) out vec4 gTexCoordcopy;

in vec2 texCoord;
in vec3 normal;
in vec3 crntPos;
in float specular;
in float diffuse;
in float fogmult;
flat in float texture_id;

uniform sampler2D textures[32];

void main(){
  gPosition=vec4(crntPos,specular);
  gNormal=vec4(normal,diffuse);
  vec3 rgb=vec3(texture(textures[int(texture_id)], texCoord));
  gTexCoord=vec4(fogmult,rgb);
  gTexCoordcopy=gTexCoord;
}