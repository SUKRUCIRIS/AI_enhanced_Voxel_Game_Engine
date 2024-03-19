#version 400 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gTexCoordo;

in vec2 texCoord;
in vec3 normal;
in vec3 crntPos;
in float specular;
in float diffuse;
in float fogmult;
in vec2 coord;
flat in float texture_id;

uniform sampler2D textures[32];

uniform sampler2D gTexCoordcopy;

uniform float time;

float mapValue(float value, float inMin, float inMax, float outMin, float outMax)
{
  return outMin + (outMax - outMin) * (value - inMin) / (inMax - inMin);
}

void main(){
  gPosition=vec4(crntPos,specular);
  gNormal=vec4(normal,diffuse);
  vec4 rgb=texture(textures[int(texture_id)], texCoord);
  float wave=0;
  wave=mapValue(sin(time+crntPos.x+crntPos.y+crntPos.z),-1,1,-0.002,0.002);
  vec4 old=texture(gTexCoordcopy, coord+vec2(wave));
  vec3 result = vec3(rgb.a) * rgb.rgb + vec3(1.0 - rgb.a) * old.gba;
  gTexCoordo=vec4(fogmult,result);
}