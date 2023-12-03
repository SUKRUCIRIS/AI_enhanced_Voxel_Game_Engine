#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "dynamic.h"
#include "physics.h"

typedef struct ins_object_manager
{
	DA *objects;
	GLuint VAO, VBO_geometry, VBO_model_matrix, VBO_normal_matrix, VBO_texture, EBO;
	DA *vertices; // 3 vertex coord, 2 texture coord, 3 normal coord
	DA *indices;
	DA *models;
	DA *normals;
	DA *textures;
	unsigned char subdata;
} ins_object_manager;

typedef struct ins_object
{
	ins_object_manager *manager;
	physic *phy;
} ins_object;

ins_object_manager *create_ins_object_manager(GLfloat *vertices, unsigned int vertex_number, GLuint *indices,
											  unsigned int indice_number);

void delete_ins_object_manager(ins_object_manager *manager);

ins_object *create_ins_object(ins_object_manager *manager, GLfloat texture_index, unsigned char has_physics,
							  unsigned char priority, float mass, float friction, float bounce);

void delete_ins_object(ins_object *obj);

void scale_ins_object(ins_object *obj, vec3 v, unsigned char effect_physic);

void rotate_ins_object(ins_object *obj, float angle, vec3 axis, unsigned char effect_physic);

void translate_ins_object(ins_object *obj, vec3 v, unsigned char effect_physic);

// after deleting or creating new objects use this before rendering
void prepare_render_ins_object_manager(ins_object_manager *manager);

void use_ins_object_manager(ins_object_manager *manager);