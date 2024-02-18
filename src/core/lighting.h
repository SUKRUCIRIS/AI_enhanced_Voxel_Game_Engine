#pragma once
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"
#include "camera.h"
#include "br_texture.h"

typedef struct lighting
{
	float zMult;
	float cascade0range;
	float cascade1range;
	float cascade2range;
	float cascade3range;
	vec4 lightColor;
	vec3 lightDir;
	vec3 center;
	vec3 up;
	float ambient;
	float specularStrength;
	GLuint shadowMapFBO, shadowMapWidth, shadowMapHeight, shadowMap;
	int windowwidth, windowheight;
	mat4 orthgonalProjection;
	mat4 lightView;
	mat4 lightProjection[4];
	camera *cam;
	vec3 lookatinput1;
	vec4 frustumcorners[8];
	vec4 frustumcenter;
	mat4 camresultinv;
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
	vec4 tmp;
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
	float fog_start;
	float fog_end;
	vec3 fog_color;
	GLuint gbufferFBO, gPosition, gNormal, gTexCoord, gdepth, quadvbo, quadvao, quadebo;
} lighting;

void calculate_lighting_projection(lighting *l, int step);

lighting *create_lighting(GLFWwindow *window, camera *cam, GLuint shadowMapWidth, GLuint shadowMapHeight, float cascade0range,
													float cascade1range, float cascade2range, float cascade3range, float fog_start, float fog_end,
													vec3 fog_color, unsigned char deferred);

// set gpass to 2 when not using deferred rendering, set textures to 0 when not using last stage deferred
void use_lighting(lighting *l, GLuint program, unsigned char shadowpass, unsigned char gpass, br_texture_manager *textures);

void delete_lighting(lighting *l);