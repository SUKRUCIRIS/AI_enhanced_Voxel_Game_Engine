#pragma once
#include "../../third_party/cglm/include/cglm/cglm.h"

// no rotate aabb 3d physics :(
// friction, drag, bounce must be in [0,1]
typedef struct physic
{
	vec3 first_minaabb, first_maxaabb;
	vec3 minaabb;			// m
	vec3 maxaabb;			// m
	vec3 center;			// m
	unsigned char priority; // 3->static, 2->kinematic, 1->dynamic
	float mass;				// kg
	vec3 forces;			// N
	vec3 acceleration;		// m/sn^2
	vec3 velocity;			// m/s
	float friction;
	float bounce;
	vec3 shouldmove;		// m
	vec3 shouldmove_2;		// m
	unsigned char disabled; // bool
	// things below will be set to 0 in every start of step, values will be set according to the collision solution
	unsigned char onfloor; // bool
	float bottom_surface_friction;
	void *obj;
	unsigned char is_br_object;
} physic;

physic *create_physic(vec3 minaabb, vec3 maxaabb, unsigned char priority, float mass,
					  float friction, float bounce, void *obj, unsigned char is_br_object);
void delete_all_physic(void);
void delete_physic(physic *ph);
unsigned char detect_collision_physic(physic *x, physic *y);
void solve_collision_physic(physic *x, physic *y, unsigned char effect_velocity);
void simulate_physic(vec3 gravity, float drag);
void apply_force(physic *ph, vec3 force);