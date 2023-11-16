#include "physics.h"
#include "dynamic.h"
#include "object.h"
#include "br_object.h"
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

DA *physic_objects = 0;

physic *create_physic(vec3 minaabb, vec3 maxaabb, unsigned char priority, float mass,
					  float friction, float bounce, void *obj, unsigned char is_br_object)
{
	physic *ph = calloc(1, sizeof(physic));
	ph->priority = priority;
	ph->mass = mass;
	ph->friction = friction;
	glm_vec3_zero(ph->acceleration);
	glm_vec3_zero(ph->velocity);
	glm_vec3_copy(minaabb, ph->minaabb);
	glm_vec3_copy(maxaabb, ph->maxaabb);
	glm_vec3_copy(minaabb, ph->first_minaabb);
	glm_vec3_copy(maxaabb, ph->first_maxaabb);
	glm_aabb_center((vec3[2]){{ph->minaabb[0], ph->minaabb[1], ph->minaabb[2]},
							  {ph->maxaabb[0], ph->maxaabb[1], ph->maxaabb[2]}},
					ph->center);
	ph->shouldmove[0] = 0;
	ph->shouldmove[1] = 0;
	ph->shouldmove[2] = 0;
	ph->obj = obj;
	if (physic_objects == 0)
	{
		physic_objects = create_DA(sizeof(physic *));
	}
	ph->bounce = bounce;
	ph->is_br_object = is_br_object;
	pushback_DA(physic_objects, &ph);
	return ph;
}

void delete_all_physic(void)
{
	if (physic_objects == 0)
	{
		return;
	}
	physic **x = 0;
	while (get_size_DA(physic_objects) > 0)
	{
		x = get_data_DA(physic_objects);
		delete_physic(x[0]);
	}
	delete_DA(physic_objects);
	physic_objects = 0;
}

void apply_force(physic *ph, vec3 force)
{
	glm_vec3_add(ph->forces, force, ph->forces);
}

void simulate_physic(vec3 gravity, float drag)
{
	if (physic_objects == 0)
	{
		return;
	}
	float milliseconds = 16;
	physic **x = get_data_DA(physic_objects);
	vec3 normal;
	for (unsigned int i = 0; i < get_size_DA(physic_objects); i++)
	{
		if (x[i]->disabled == 1)
		{
			continue;
		}
		if (x[i]->priority == 1)
		{
			glm_vec3_normalize_to(x[i]->velocity, normal);
			glm_vec3_scale(normal, -drag * glm_vec3_norm(x[i]->velocity) * glm_vec3_norm(x[i]->velocity), normal);
			apply_force(x[i], normal);
			if (x[i]->onfloor == 0)
			{
				glm_vec3_scale(gravity, x[i]->mass, normal);
				apply_force(x[i], normal);
			}
			else
			{
				glm_vec3_normalize_to(x[i]->velocity, normal);
				glm_vec3_scale(normal, -gravity[1] * x[i]->mass * (x[i]->friction + x[i]->bottom_surface_friction) / 2, normal);
				apply_force(x[i], normal);
			}
		}
		glm_vec3_scale(x[i]->forces, 1 / x[i]->mass, normal);
		glm_vec3_copy(GLM_VEC3_ZERO, x[i]->forces);
		glm_vec3_add(normal, x[i]->acceleration, x[i]->acceleration);
		glm_vec3_scale(x[i]->acceleration, milliseconds / 1000, normal);
		glm_vec3_add(normal, x[i]->velocity, x[i]->velocity);
		glm_vec3_scale(x[i]->velocity, milliseconds / 1000, x[i]->shouldmove_2);
		if (fabsf(x[i]->velocity[0]) < 0.1f)
		{
			x[i]->velocity[0] = 0;
		}
		if (fabsf(x[i]->velocity[1]) < 0.1f)
		{
			x[i]->velocity[1] = 0;
		}
		if (fabsf(x[i]->velocity[2]) < 0.1f)
		{
			x[i]->velocity[2] = 0;
		}
		x[i]->onfloor = 0;
		x[i]->bottom_surface_friction = 0;
	}
	int iteration = 1;
	for (unsigned int i = 0; i < get_size_DA(physic_objects); i++)
	{
		if (x[i]->disabled == 1)
		{
			continue;
		}
		iteration = (int)(glm_vec3_norm(x[i]->shouldmove_2) + 1);
		glm_vec3_scale(x[i]->shouldmove_2, 1.0f / iteration, x[i]->shouldmove_2);
		for (int i3 = 0; i3 < iteration; i3++)
		{
			if (x[i]->obj)
			{
				if (x[i]->is_br_object)
				{
					translate_br_object(x[i]->obj, x[i]->shouldmove_2, 1);
				}
				else
				{
					translate_object(x[i]->obj, x[i]->shouldmove_2, 1);
				}
			}
			else
			{
				glm_vec3_add(x[i]->minaabb, x[i]->shouldmove_2, x[i]->minaabb);
				glm_vec3_add(x[i]->maxaabb, x[i]->shouldmove_2, x[i]->maxaabb);
			}
			for (unsigned int i2 = 0; i2 < get_size_DA(physic_objects); i2++)
			{
				if (i == i2 || x[i2]->disabled == 1)
				{
					continue;
				}
				if (detect_collision_physic(x[i], x[i2]))
				{
					if (i3 == iteration - 1)
					{
						solve_collision_physic(x[i], x[i2], 1);
					}
					else
					{
						solve_collision_physic(x[i], x[i2], 0);
					}
				}
			}
		}
	}
}

void delete_physic(physic *ph)
{ // this wont delete object
	if (ph)
	{
		remove_DA(physic_objects, get_index_DA(physic_objects, &ph));
		free(ph);
	}
}

unsigned char detect_collision_physic(physic *x, physic *y)
{
	return glm_aabb_aabb((vec3[2]){{x->minaabb[0], x->minaabb[1], x->minaabb[2]},
								   {x->maxaabb[0], x->maxaabb[1], x->maxaabb[2]}},
						 (vec3[2]){{y->minaabb[0], y->minaabb[1], y->minaabb[2]},
								   {y->maxaabb[0], y->maxaabb[1], y->maxaabb[2]}});
}

void solve_collision_physic(physic *x, physic *y, unsigned char effect_velocity)
{
	if (x->priority == y->priority && x->priority == 3)
	{
		return;
	}
	glm_aabb_center((vec3[2]){{x->minaabb[0], x->minaabb[1], x->minaabb[2]},
							  {x->maxaabb[0], x->maxaabb[1], x->maxaabb[2]}},
					x->center);
	glm_aabb_center((vec3[2]){{y->minaabb[0], y->minaabb[1], y->minaabb[2]},
							  {y->maxaabb[0], y->maxaabb[1], y->maxaabb[2]}},
					y->center);
	vec3 overlap;
	overlap[0] = (x->maxaabb[0] - x->minaabb[0]) / 2.0f + (y->maxaabb[0] - y->minaabb[0]) / 2.0f - fabsf(x->center[0] - y->center[0]);
	overlap[1] = (x->maxaabb[1] - x->minaabb[1]) / 2.0f + (y->maxaabb[1] - y->minaabb[1]) / 2.0f - fabsf(x->center[1] - y->center[1]);
	overlap[2] = (x->maxaabb[2] - x->minaabb[2]) / 2.0f + (y->maxaabb[2] - y->minaabb[2]) / 2.0f - fabsf(x->center[2] - y->center[2]);
	vec3 collision_speed_mul;
	vec3 collision_overlap_mul;
	int axis = 0;
	if (min(min(overlap[0], overlap[1]), overlap[2]) == overlap[0])
	{
		collision_speed_mul[0] = -(x->bounce + y->bounce) / 2;
		collision_speed_mul[1] = 1;
		collision_speed_mul[2] = 1;

		collision_overlap_mul[0] = 1;
		collision_overlap_mul[1] = 0;
		collision_overlap_mul[2] = 0;
		axis = 0;
	}
	else if (min(min(overlap[0], overlap[1]), overlap[2]) == overlap[1])
	{
		collision_speed_mul[0] = 1;
		collision_speed_mul[1] = -(x->bounce + y->bounce) / 2;
		collision_speed_mul[2] = 1;

		collision_overlap_mul[0] = 0;
		collision_overlap_mul[1] = 1;
		collision_overlap_mul[2] = 0;
		axis = 1;
	}
	else
	{
		collision_speed_mul[0] = 1;
		collision_speed_mul[1] = 1;
		collision_speed_mul[2] = -(x->bounce + y->bounce) / 2;

		collision_overlap_mul[0] = 0;
		collision_overlap_mul[1] = 0;
		collision_overlap_mul[2] = 1;
		axis = 2;
	}
	if (x->priority == y->priority)
	{
		if (x->center[axis] > y->center[axis])
		{
			glm_vec3_mul(collision_overlap_mul, overlap, x->shouldmove);
			glm_vec3_scale(x->shouldmove, 0.50f, x->shouldmove);

			glm_vec3_mul(collision_overlap_mul, overlap, y->shouldmove);
			glm_vec3_scale(y->shouldmove, -0.50f, y->shouldmove);
			if (axis == 1)
			{
				x->onfloor = 1;
				x->bottom_surface_friction = y->friction;
			}
		}
		else
		{
			glm_vec3_mul(collision_overlap_mul, overlap, x->shouldmove);
			glm_vec3_scale(x->shouldmove, -0.50f, x->shouldmove);

			glm_vec3_mul(collision_overlap_mul, overlap, y->shouldmove);
			glm_vec3_scale(y->shouldmove, 0.50f, y->shouldmove);
			if (axis == 1)
			{
				y->onfloor = 1;
				y->bottom_surface_friction = x->friction;
			}
		}
		if (x->obj)
		{
			if (x->is_br_object)
			{
				translate_br_object(x->obj, x->shouldmove, 1);
			}
			else
			{
				translate_object(x->obj, x->shouldmove, 1);
			}
		}
		else
		{
			glm_vec3_add(x->minaabb, x->shouldmove, x->minaabb);
			glm_vec3_add(x->maxaabb, x->shouldmove, x->maxaabb);
		}
		if (y->obj)
		{
			if (y->is_br_object)
			{
				translate_br_object(y->obj, y->shouldmove, 1);
			}
			else
			{
				translate_object(y->obj, y->shouldmove, 1);
			}
		}
		else
		{
			glm_vec3_add(y->minaabb, y->shouldmove, y->minaabb);
			glm_vec3_add(y->maxaabb, y->shouldmove, y->maxaabb);
		}
		if (x->priority == 1 && effect_velocity)
		{
			// elastic collision
			vec3 tmp1;
			vec3 tmp2;
			vec3 oldx;
			vec3 oldy;
			glm_vec3_copy(x->velocity, oldx);
			glm_vec3_copy(y->velocity, oldy);

			glm_vec3_scale(oldx, x->mass - y->mass, tmp1);
			glm_vec3_scale(oldy, 2 * y->mass, tmp2);
			glm_vec3_add(tmp1, tmp2, tmp1);
			glm_vec3_scale(tmp1, 1 / (x->mass + y->mass), tmp1);
			glm_vec3_copy(tmp1, x->velocity);

			glm_vec3_scale(oldy, y->mass - x->mass, tmp1);
			glm_vec3_scale(oldx, 2 * x->mass, tmp2);
			glm_vec3_add(tmp1, tmp2, tmp1);
			glm_vec3_scale(tmp1, 1 / (x->mass + y->mass), tmp1);
			glm_vec3_copy(tmp1, y->velocity);

			glm_vec3_copy(GLM_VEC3_ZERO, x->acceleration);
			glm_vec3_copy(GLM_VEC3_ZERO, y->acceleration);
		}
	}
	else if (x->priority > y->priority)
	{
		if (x->center[axis] > y->center[axis])
		{
			glm_vec3_mul(collision_overlap_mul, overlap, y->shouldmove);
			glm_vec3_scale(y->shouldmove, -1, y->shouldmove);
			if (y->velocity[axis] > 0 && effect_velocity)
			{
				glm_vec3_mul(y->velocity, collision_speed_mul, y->velocity);
			}
			else if (y->velocity[axis] == 0 && effect_velocity)
			{
				y->acceleration[axis] = 0;
				y->forces[axis] = 0;
			}
		}
		else
		{
			glm_vec3_mul(collision_overlap_mul, overlap, y->shouldmove);
			if (axis == 1)
			{
				y->onfloor = 1;
				y->bottom_surface_friction = x->friction;
			}
			if (y->velocity[axis] < 0 && effect_velocity)
			{
				glm_vec3_mul(y->velocity, collision_speed_mul, y->velocity);
			}
			else if (y->velocity[axis] == 0 && effect_velocity)
			{
				y->acceleration[axis] = 0;
				y->forces[axis] = 0;
			}
		}
		if (y->obj)
		{
			if (y->is_br_object)
			{
				translate_br_object(y->obj, y->shouldmove, 1);
			}
			else
			{
				translate_object(y->obj, y->shouldmove, 1);
			}
		}
		else
		{
			glm_vec3_add(y->minaabb, y->shouldmove, y->minaabb);
			glm_vec3_add(y->maxaabb, y->shouldmove, y->maxaabb);
		}
	}
	else
	{
		if (x->center[axis] > y->center[axis])
		{
			glm_vec3_mul(collision_overlap_mul, overlap, x->shouldmove);
			if (axis == 1)
			{
				x->onfloor = 1;
				x->bottom_surface_friction = y->friction;
			}
			if (x->velocity[axis] < 0 && effect_velocity)
			{
				glm_vec3_mul(x->velocity, collision_speed_mul, x->velocity);
			}
			else if (x->velocity[axis] == 0 && effect_velocity)
			{
				x->acceleration[axis] = 0;
				x->forces[axis] = 0;
			}
		}
		else
		{
			glm_vec3_mul(collision_overlap_mul, overlap, x->shouldmove);
			glm_vec3_scale(x->shouldmove, -1, x->shouldmove);
			if (x->velocity[axis] > 0 && effect_velocity)
			{
				glm_vec3_mul(x->velocity, collision_speed_mul, x->velocity);
			}
			else if (x->velocity[axis] == 0 && effect_velocity)
			{
				x->acceleration[axis] = 0;
				x->forces[axis] = 0;
			}
		}
		if (x->obj)
		{
			if (x->is_br_object)
			{
				translate_br_object(x->obj, x->shouldmove, 1);
			}
			else
			{
				translate_object(x->obj, x->shouldmove, 1);
			}
		}
		else
		{
			glm_vec3_add(x->minaabb, x->shouldmove, x->minaabb);
			glm_vec3_add(x->maxaabb, x->shouldmove, x->maxaabb);
		}
	}
}