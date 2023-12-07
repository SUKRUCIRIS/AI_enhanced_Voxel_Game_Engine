#pragma once
#include "br_object.h"
#include "br_texture.h"

void load_object_br(br_object_manager *obj_manager, br_texture_manager *text_manager,
					const char *path, float texture_start_index, unsigned char has_physics,
					unsigned char priority, float mass, float friction, float bounce);