#pragma once
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"
#include "camera.h"

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
} lighting;

void calculate_lighting_projection(lighting *l, int step);

lighting *create_lighting(GLFWwindow *window, camera *cam, GLuint shadowMapWidth, GLuint shadowMapHeight, float cascade0range,
						  float cascade1range, float cascade2range, float cascade3range);

void use_lighting(lighting *l, GLuint program, unsigned char shadowpass);

void delete_lighting(lighting *l);