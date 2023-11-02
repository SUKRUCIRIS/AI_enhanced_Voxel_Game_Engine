#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"
#include "physics.h"

typedef struct br_object_manager
{
	DA *objects;
	GLuint VAO, VBO, EBO;
	DA *vertices; // 3 vertex coord, 2 texture coord, 3 normal coord, 1 texture id
	DA *indices;
} br_object_manager;

typedef struct br_object // batch rendering object
{
	br_object_manager *manager;
	unsigned int vertex_number;
	unsigned int indice_number;
	unsigned int vertex_start;
	unsigned int indice_start;
	mat4 model;
	mat4 normal;
	physic *phy;
} br_object;

br_object_manager *create_br_manager(void);
void delete_br_manager(br_object_manager *manager);
br_object *create_br_object(br_object_manager *manager, GLfloat *vertices, unsigned int vertex_number, GLuint *indices,
							unsigned int indice_number, GLfloat texture_index, unsigned char has_physics,
							unsigned char priority, float mass, float friction, float bounce);
void delete_br_object(br_object *obj);
void scale_br_object(br_object *obj, vec3 v, unsigned char effect_physic);
void rotate_br_object(br_object *obj, float angle, vec3 axis, unsigned char effect_physic);
void translate_br_object(br_object *obj, vec3 v, unsigned char effect_physic);
void prepare_render_br_manager(br_object_manager *manager);
void use_br_object_manager(br_object_manager *manager);