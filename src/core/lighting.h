#pragma once
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"
#include "camera.h"

typedef struct lighting
{
	vec4 lightColor;
	vec3 lightDir;
	float ambient;
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
} lighting;

lighting *create_lighting(void); // set variables from struct directly, dont touch DAs

void use_lighting(lighting *l, GLuint program);

void delete_lighting(lighting *l);