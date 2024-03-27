#version 400 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTexCoord;
uniform sampler2D ssao;

uniform vec4 lightColor;
uniform float ambient;
uniform vec3 fog_color;

uniform vec3 lightDir;
uniform float specularStrength;
uniform vec3 camPos;
uniform float fog_start;
uniform float fog_end;

uniform mat4 view;
uniform mat4 lightProjection[4];
uniform float cascade0range;
uniform float cascade1range;
uniform float cascade2range;
uniform float cascade3range;
uniform sampler2DArrayShadow shadowMap;

uniform int has_ssao;

void main(){
	vec4 gpos=texture(gPosition, TexCoords);
	vec4 gnorm=texture(gNormal, TexCoords);
	vec4 gtex=texture(gTexCoord, TexCoords);
	float AmbientOcclusion = 1;
	
	if(has_ssao==1){
		AmbientOcclusion=texture(ssao, TexCoords).r;
	}

  vec3 rgb=gtex.xyz;
  vec3 normal=gnorm.xyz;
  vec3 crntPos=gpos.xyz;
  float specular=0;
  float diffuse=0;
  float fogmult=0;

	if(length(normal)==0){
		FragColor=vec4(rgb,1);
		return;
	}

	vec3 lightDirection = normalize(lightDir);
	diffuse = max(dot(normal, lightDirection), 0.0f);

	if(diffuse!=0){
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 2);
		specular = specAmount * specularStrength;
	}
	float dist = distance(vec3(crntPos.x,0,crntPos.z), vec3(camPos.x,0,camPos.z));

	if(dist >= fog_start){
		if(dist >= fog_end){
			fogmult = 1;
		}
		else{
			fogmult = (dist - fog_start) / (fog_end - fog_start);
		}
	}
	else{
		fogmult = 0;
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

	vec4 color = vec4(rgb,1) * lightColor * (diffuse * (1.0f - shadow) + ambient + specular * (1.0f - shadow)) * AmbientOcclusion;
	FragColor.xyz = color.xyz * (1 - fogmult) + fog_color * fogmult;
	FragColor.a = 1.0f;
	//FragColor=vec4(AmbientOcclusion,AmbientOcclusion,AmbientOcclusion,1);
}