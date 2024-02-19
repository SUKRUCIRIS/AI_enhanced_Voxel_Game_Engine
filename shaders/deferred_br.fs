#version 400 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTexCoord;
uniform sampler2D ssao;

uniform sampler2D textures[32];
uniform vec4 lightColor;
uniform float ambient;
uniform vec3 fog_color;

uniform mat4 view;
uniform mat4 lightProjection[4];
uniform float cascade0range;
uniform float cascade1range;
uniform float cascade2range;
uniform float cascade3range;
uniform sampler2DArrayShadow shadowMap;

void main(){
	vec4 gpos=texture(gPosition, TexCoords);
	vec4 gnorm=texture(gNormal, TexCoords);
	vec4 gtex=texture(gTexCoord, TexCoords);
	float AmbientOcclusion = texture(ssao, TexCoords).r;

  vec2 texCoord=gtex.xy;
  vec3 normal=gnorm.xyz;
  vec3 crntPos=gpos.xyz;
  float specular=gpos.w;
  float diffuse=gnorm.w;
  float fogmult=gtex.z;
  float texture_id=gtex.w;

	if(length(normal)==0){
		FragColor=vec4(0.718f, 0.702f, 0.671f, 1.0f);
		return;
	}

	float depthValue = abs((view*vec4(crntPos,1.0f)).z);
	int layer = -1;
	if(depthValue<=cascade0range){
		layer=0;
	}
	else if(depthValue<=cascade1range){
		layer=1;
	}
	else if(depthValue<=cascade2range){
		layer=2;
	}
	else if(depthValue<=cascade3range){
		layer=3;
	}

	vec4 fragPosLight=vec4(crntPos,1.0f);
	if(layer!=-1){
		fragPosLight=lightProjection[layer]*fragPosLight;
	}

	float shadow = 0;
	if(layer!=-1){
		vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
		if(lightCoords.z <= 1.0f)
		{
			lightCoords = (lightCoords + 1.0f) / 2.0f;
			float currentDepth = lightCoords.z;
			int sampleRadius = 3;
			vec2 pixelSize = 1.0 / vec2(textureSize(shadowMap, 0));
			for(int y = -sampleRadius; y <= sampleRadius; y++)
			{
				for(int x = -sampleRadius; x <= sampleRadius; x++)
				{
					float closestDepth = texture(shadowMap, vec4(lightCoords.xy + vec2(x, y) * pixelSize, layer, 1));
					if (currentDepth > closestDepth){
						shadow += 1.0f;     
					}
				}    
			}
			shadow /= pow((sampleRadius * 2 + 1), 2);
		}
	}

	vec4 color = texture(textures[int(texture_id)], texCoord) * lightColor * (diffuse * (1.0f - shadow) + (ambient * AmbientOcclusion) + specular * (1.0f - shadow));
	FragColor.xyz = color.xyz * (1 - fogmult) + fog_color * fogmult;
	FragColor.a = 1.0f;
	//FragColor=vec4(AmbientOcclusion,AmbientOcclusion,AmbientOcclusion,1);
}