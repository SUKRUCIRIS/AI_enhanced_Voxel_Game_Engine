#version 330 core
out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
in vec3 crntPos;
in vec4 fragPosLight;

uniform sampler2D tex0;
uniform sampler2D shadowMap;

uniform vec4 lightColor;
uniform vec3 lightDir;
uniform float ambient;
uniform float specularStrength;
uniform vec3 camPos;
uniform float shininess;

void main(){
	vec3 lightDirection = normalize(lightDir);
	float diffuse = max(dot(normal, lightDirection), 0.0f);


	float specular=0;
	if(diffuse!=0){
		vec3 viewDirection = normalize(camPos - crntPos);
		vec3 reflectionDirection = reflect(-lightDirection, normal);
		float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), shininess);
		specular = specAmount * specularStrength;
	}

	float shadow = 0.0f;
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;

		float bias = max(0.025f * (1.0f - dot(normal, lightDirection)), 0.00028f);

		if(normal.y > 0.97){
			bias=0;
		}

		int sampleRadius = 5;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}

	diffuse = diffuse * (1.0f - shadow);
	specular = specular * (1.0f - shadow);
	FragColor = texture(tex0, texCoord) * lightColor * (diffuse + ambient + specular);

	FragColor.a = 1.0f;
}