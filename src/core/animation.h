#pragma once
#include "br_object.h"

void init_animations(void);

void add_animation_translate_br_object(br_object *obj, vec3 v, unsigned char effect_physic, double durationms);

void add_animation_translate_br_manager(br_object_manager *manager, vec3 v, unsigned char effect_physic, double durationms);

void play_animations(void);

void delete_animations(void);