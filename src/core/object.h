#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "camera.h"
#include "dynamic.h"
typedef struct physic physic;
typedef struct object
{
	GLuint VAO, VBO, EBO;
	unsigned char copy;
	unsigned int vertex_number;
	unsigned int indice_number;
	mat4 model;
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
	physic *phy;
} object;

object *create_object(GLfloat *vertices, unsigned int vertex_number, GLenum usage, GLuint *indices,
					  unsigned int indice_number, unsigned char is_tex_vertex, unsigned char has_physics,
					  unsigned char priority, float mass, float friction, float bounce);
void delete_object(object *obj);
void delete_all_object(void);
void use_object(object *obj, GLuint program, camera *cam);
void scale_object(object *obj, vec3 v, unsigned char effect_physic);
void rotate_object(object *obj, float angle, vec3 axis, unsigned char effect_physic);
void translate_object(object *obj, vec3 v, unsigned char effect_physic);
object *create_object_copy(object *obj, unsigned char has_physics);