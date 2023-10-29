#pragma once
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"
#include "camera.h"

typedef struct lighting
{
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
	mat4 lightProjection;
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
} lighting;

lighting *create_lighting(GLFWwindow *window);

void use_lighting(lighting *l, GLuint program, unsigned char shadowpass);

void delete_lighting(lighting *l);