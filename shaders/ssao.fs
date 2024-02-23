#version 400 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

uniform vec2 noiseScale; 

uniform mat4 projection;
uniform mat4 view;

void main()
{
  vec3 fragPos = vec3(view * vec4(texture(gPosition, TexCoords).xyz, 1.0f));
  vec3 normal = mat3(view) * normalize(texture(gNormal, TexCoords).rgb);
  vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

  vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
  vec3 bitangent = cross(normal, tangent);
  mat3 TBN = mat3(tangent, bitangent, normal);

  float occlusion = 0.0;
  for(int i = 0; i < kernelSize; ++i)
  {
    vec3 samplePos = TBN * samples[i];
    samplePos = fragPos + samplePos * radius; 

    //if (length(samplePos - fragPos) > 4) break;
    vec4 offset = vec4(samplePos, 1.0);
    offset = projection * offset;
    offset.xyz /= offset.w;
    offset.xyz = offset.xyz * 0.5 + 0.5;

    float sampleDepth = vec3(view * vec4(texture(gPosition, offset.xy).xyz, 1.0f)).z;

    float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
    occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
  }
  occlusion = 1 - (occlusion / kernelSize);

  FragColor = occlusion;
}