#pragma once
#include "br_object.h"
#include "br_texture.h"
#include "../../third_party/assimp/include/assimp/scene.h"

typedef struct br_scene
{
	br_object **meshes;
	br_texture **textures;
	unsigned int mesh_count;
	unsigned int texture_count;
} br_scene;

br_scene load_object_br(br_object_manager *obj_manager, br_texture_manager *text_manager,
						struct aiScene *scene, float texture_start_index, unsigned char has_physics,
						unsigned char priority, float mass, float friction, float bounce);

struct aiScene *load_model(const char *path, unsigned char flip_order);

void free_model(struct aiScene *scene);