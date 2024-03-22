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
	unsigned char subdata;
	mat4 translation;
	mat4 rotation;
	mat4 scale;
	mat4 model;
	mat4 normal;
	DA *programs; // i will save uniforms here. i wont find their locations everytime i render for performance
	DA *uniforms;
	unsigned int object_number;
	unsigned int indice_number;
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

br_object_manager *create_br_object_manager(void);

void delete_br_object_manager(br_object_manager *manager);

br_object *create_br_object(br_object_manager *manager, GLfloat *vertices, unsigned int vertex_number, GLuint *indices,
														unsigned int indice_number, GLfloat texture_index, unsigned char has_physics,
														unsigned char priority, float mass, float friction, float bounce);

void delete_br_object(br_object *obj);

void scale_br_object(br_object *obj, vec3 v, unsigned char effect_physic);

void rotate_br_object(br_object *obj, float angle, vec3 axis, unsigned char effect_physic);

void translate_br_object(br_object *obj, vec3 v, unsigned char effect_physic);

void scale_br_object_all(br_object_manager *manager, vec3 v);

void rotate_br_object_all(br_object_manager *manager, float angle, vec3 axis);

void translate_br_object_all(br_object_manager *manager, vec3 v);

// after deleting or creating new objects use this before rendering
void prepare_render_br_object_manager(br_object_manager *manager);

void use_br_object_manager(br_object_manager *manager, GLuint program);

void delete_cpu_memory_br_object_manager(br_object_manager *manager);

void set_position_br_object_all(br_object_manager *manager, vec3 v);

void get_position_br_object_all(br_object_manager *manager, vec3 v);

void set_rotation_br_object_all(br_object_manager *manager, float angle, vec3 axis);