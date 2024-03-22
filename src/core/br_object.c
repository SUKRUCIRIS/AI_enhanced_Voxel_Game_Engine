#include "br_object.h"

void prepare_render_br_object_manager(br_object_manager *manager)
{
	glDeleteVertexArrays(1, &(manager->VAO));
	glDeleteBuffers(1, &(manager->VBO));
	glDeleteBuffers(1, &(manager->EBO));
	glBindVertexArray(0);
	if (get_size_DA(manager->objects) > 0)
	{
		glGenVertexArrays(1, &(manager->VAO));
		glGenBuffers(1, &(manager->VBO));
		glGenBuffers(1, &(manager->EBO));
		glBindVertexArray(manager->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, manager->VBO);
		glBufferData(GL_ARRAY_BUFFER, get_size_DA(manager->vertices) * sizeof(GLfloat), get_data_DA(manager->vertices), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(GLfloat), (void *)(8 * sizeof(GLfloat)));
		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, manager->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, get_size_DA(manager->indices) * sizeof(GLuint), get_data_DA(manager->indices), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	manager->subdata = 0;
}

br_object_manager *create_br_object_manager(void)
{
	br_object_manager *x = malloc(sizeof(br_object_manager));
	x->VAO = 0;
	x->VBO = 0;
	x->EBO = 0;
	x->objects = create_DA_HIGH_MEMORY(sizeof(br_object *));
	x->vertices = create_DA_HIGH_MEMORY(sizeof(GLfloat));
	x->indices = create_DA_HIGH_MEMORY(sizeof(GLuint));
	x->subdata = 0;
	glm_mat4_copy(GLM_MAT4_IDENTITY, x->model);
	glm_mat4_copy(GLM_MAT4_IDENTITY, x->normal);
	glm_mat4_copy(GLM_MAT4_IDENTITY, x->translation);
	glm_mat4_copy(GLM_MAT4_IDENTITY, x->rotation);
	glm_mat4_copy(GLM_MAT4_IDENTITY, x->scale);
	x->programs = create_DA(sizeof(GLuint));
	x->uniforms = create_DA(sizeof(GLint));
	x->object_number = 0;
	x->indice_number = 0;
	return x;
}

void delete_br_object_manager(br_object_manager *manager)
{
	if (manager->objects != 0)
	{
		br_object **objects = get_data_DA(manager->objects);
		for (unsigned int i = 0; i < get_size_DA(manager->objects); i++)
		{
			delete_physic(objects[i]->phy);
			free(objects[i]);
		}
	}
	delete_DA(manager->objects);
	delete_DA(manager->vertices);
	delete_DA(manager->indices);
	delete_DA(manager->programs);
	delete_DA(manager->uniforms);
	glDeleteVertexArrays(1, &(manager->VAO));
	glDeleteBuffers(1, &(manager->VBO));
	glDeleteBuffers(1, &(manager->EBO));
	free(manager);
}

br_object *create_br_object(br_object_manager *manager, GLfloat *vertices, unsigned int vertex_number, GLuint *indices,
														unsigned int indice_number, GLfloat texture_index, unsigned char has_physics,
														unsigned char priority, float mass, float friction, float bounce)
{
	if (manager == 0 || vertices == 0 || vertex_number == 0)
	{
		return 0;
	}
	GLuint *noindice = 0;
	if (indices == 0 || indice_number == 0)
	{
		indice_number = vertex_number;
		noindice = (GLuint *)malloc(sizeof(GLuint) * indice_number);
		for (unsigned int i = 0; i < indice_number; i++)
		{
			noindice[i] = i;
		}
		indices = noindice;
	}
	br_object *x = malloc(sizeof(br_object));
	glm_mat4_copy(GLM_MAT4_IDENTITY, x->model);
	glm_mat4_copy(GLM_MAT4_IDENTITY, x->normal);
	x->manager = manager;
	x->vertex_number = vertex_number;
	x->indice_number = indice_number;
	pushback_DA(x->manager->objects, &x);
	if (has_physics == 1)
	{
		vec3 minaabb, maxaabb;
		for (unsigned int i = 0; i < vertex_number; i++)
		{
			if (i == 0)
			{
				minaabb[0] = vertices[0];
				minaabb[1] = vertices[1];
				minaabb[2] = vertices[2];
				maxaabb[0] = vertices[0];
				maxaabb[1] = vertices[1];
				maxaabb[2] = vertices[2];
			}
			else
			{
				if (vertices[i * 9] > maxaabb[0])
				{
					maxaabb[0] = vertices[i * 9];
				}
				if (vertices[i * 9 + 1] > maxaabb[1])
				{
					maxaabb[1] = vertices[i * 9 + 1];
				}
				if (vertices[i * 9 + 2] > maxaabb[2])
				{
					maxaabb[2] = vertices[i * 9 + 2];
				}
				if (vertices[i * 9] < minaabb[0])
				{
					minaabb[0] = vertices[i * 9];
				}
				if (vertices[i * 9 + 1] < minaabb[1])
				{
					minaabb[1] = vertices[i * 9 + 1];
				}
				if (vertices[i * 9 + 2] < minaabb[2])
				{
					minaabb[2] = vertices[i * 9 + 2];
				}
			}
		}
		x->phy = create_physic(minaabb, maxaabb, priority, mass, friction, bounce, x, 1);
	}
	else
	{
		x->phy = 0;
	}
	GLfloat oldtext_index = vertices[8];
	for (unsigned int i = 0; i < vertex_number; i++)
	{
		vertices[9 * i + 8] = texture_index;
	}
	x->vertex_start = get_size_DA(x->manager->vertices) / 9;
	x->indice_start = get_size_DA(x->manager->indices);
	pushback_many_DA(x->manager->vertices, vertices, vertex_number * 9);
	for (unsigned int i = 0; i < indice_number; i++)
	{
		indices[i] += x->vertex_start;
	}
	pushback_many_DA(x->manager->indices, indices, indice_number);
	for (unsigned int i = 0; i < indice_number; i++)
	{
		indices[i] -= x->vertex_start;
	}
	for (unsigned int i = 0; i < vertex_number; i++)
	{
		vertices[9 * i + 8] = oldtext_index;
	}
	free(noindice);
	manager->indice_number = get_size_DA(manager->indices);
	manager->object_number = get_size_DA(manager->objects);
	return x;
}

void delete_br_object(br_object *obj)
{
	delete_physic(obj->phy);
	remove_many_DA(obj->manager->vertices, obj->vertex_start * 9, obj->vertex_start * 9 - 1 + obj->vertex_number * 9);
	remove_many_DA(obj->manager->indices, obj->indice_start, obj->indice_start - 1 + obj->indice_number);
	unsigned int index = get_index_DA(obj->manager->objects, &obj);
	GLuint *indices = get_data_DA(obj->manager->indices);
	if (index != get_size_DA(obj->manager->objects) - 1)
	{
		br_object **objs = get_data_DA(obj->manager->objects);
		for (unsigned int i = index + 1; i < get_size_DA(obj->manager->objects); i++)
		{
			objs[i]->vertex_start -= obj->vertex_number;
			objs[i]->indice_start -= obj->indice_number;

			for (unsigned int i2 = objs[i]->indice_start; i2 < objs[i]->indice_start + objs[i]->indice_number; i2++)
			{
				indices[i2] -= obj->vertex_number;
			}
		}
	}
	remove_DA(obj->manager->objects, index);
	free(obj);
}

void update_br_physic(br_object *obj)
{
	vec4 result;
	glm_mat4_mulv(obj->model, (vec4){obj->phy->first_minaabb[0], obj->phy->first_minaabb[1], obj->phy->first_minaabb[2], 1}, result);
	glm_vec3_copy(result, obj->phy->minaabb);
	glm_mat4_mulv(obj->model, (vec4){obj->phy->first_maxaabb[0], obj->phy->first_maxaabb[1], obj->phy->first_maxaabb[2], 1}, result);
	glm_vec3_copy(result, obj->phy->maxaabb);
	for (int i = 0; i < 3; i++)
	{
		if (obj->phy->minaabb[i] > obj->phy->maxaabb[i])
		{
			result[i] = obj->phy->minaabb[i];
			obj->phy->minaabb[i] = obj->phy->maxaabb[i];
			obj->phy->maxaabb[i] = result[i];
		}
	}
}

void apply_model_matrix(br_object *obj)
{
	glm_mat4_inv(obj->model, obj->normal);
	glm_mat4_transpose(obj->normal);
	vec4 tmp = {0, 0, 0, 0};
	GLfloat *vertices = get_data_DA(obj->manager->vertices);
	for (unsigned int i = 0; i < obj->vertex_number; i++)
	{
		glm_mat4_mulv(obj->model,
									(vec4){vertices[(obj->vertex_start + i) * 9], vertices[(obj->vertex_start + i) * 9 + 1], vertices[(obj->vertex_start + i) * 9 + 2], 1},
									tmp);
		vertices[(obj->vertex_start + i) * 9] = tmp[0];
		vertices[(obj->vertex_start + i) * 9 + 1] = tmp[1];
		vertices[(obj->vertex_start + i) * 9 + 2] = tmp[2];

		glm_mat4_mulv(obj->normal,
									(vec4){vertices[(obj->vertex_start + i) * 9 + 5], vertices[(obj->vertex_start + i) * 9 + 6], vertices[(obj->vertex_start + i) * 9 + 7], 1},
									tmp);
		vertices[(obj->vertex_start + i) * 9 + 5] = tmp[0];
		vertices[(obj->vertex_start + i) * 9 + 6] = tmp[1];
		vertices[(obj->vertex_start + i) * 9 + 7] = tmp[2];
	}
	glm_mat4_copy(GLM_MAT4_IDENTITY, obj->model);
	glm_mat4_copy(GLM_MAT4_IDENTITY, obj->normal);
	obj->manager->subdata = 1;
}

void scale_br_object(br_object *obj, vec3 v, unsigned char effect_physic)
{
	glm_scale(obj->model, v);
	if (effect_physic == 1 && obj->phy != 0)
	{
		update_br_physic(obj);
	}

	apply_model_matrix(obj);
}

void rotate_br_object(br_object *obj, float angle, vec3 axis, unsigned char effect_physic)
{
	glm_rotate(obj->model, glm_rad(angle), axis);
	if (effect_physic == 1 && obj->phy != 0)
	{
		update_br_physic(obj);
	}

	apply_model_matrix(obj);
}

void translate_br_object(br_object *obj, vec3 v, unsigned char effect_physic)
{
	glm_translate(obj->model, v);
	if (effect_physic == 1 && obj->phy != 0)
	{
		update_br_physic(obj);
	}

	apply_model_matrix(obj);
}

void scale_br_object_all(br_object_manager *manager, vec3 v)
{
	mat4 tmp;
	glm_scale_make(tmp, v);
	glm_mat4_mul(tmp, manager->scale, manager->scale);
}

void rotate_br_object_all(br_object_manager *manager, float angle, vec3 axis)
{
	mat4 tmp;
	glm_rotate_make(tmp, glm_rad(angle), axis);
	glm_mat4_mul(tmp, manager->rotation, manager->rotation);
}

void translate_br_object_all(br_object_manager *manager, vec3 v)
{
	glm_vec3_add(v, manager->translation[3], manager->translation[3]);
}

void use_br_object_manager(br_object_manager *manager, GLuint program)
{
	if (manager->object_number > 0)
	{
		if (get_index_DA(manager->programs, &program) == UINT_MAX)
		{
			pushback_DA(manager->programs, &program);
			GLint uniform = glGetUniformLocation(program, "model");
			pushback_DA(manager->uniforms, &uniform);
			uniform = glGetUniformLocation(program, "normalMatrix");
			pushback_DA(manager->uniforms, &uniform);
		}
		GLint *uniforms = get_data_DA(manager->uniforms);
		glm_mat4_mulN((mat4 *[]){&manager->translation, &manager->rotation, &manager->scale}, 3, manager->model);
		glm_mat4_inv(manager->model, manager->normal);
		glm_mat4_transpose(manager->normal);
		glUniformMatrix4fv(uniforms[get_index_DA(manager->programs, &program) * 2], 1, GL_FALSE, manager->model[0]);
		glUniformMatrix4fv(uniforms[get_index_DA(manager->programs, &program) * 2 + 1], 1, GL_FALSE, manager->normal[0]);

		if (manager->subdata == 1)
		{
			glBindBuffer(GL_ARRAY_BUFFER, manager->VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, get_size_DA(manager->vertices) * sizeof(GLfloat), get_data_DA(manager->vertices));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			manager->subdata = 0;
		}

		glBindVertexArray(manager->VAO);
		glDrawElements(GL_TRIANGLES, manager->indice_number, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void delete_cpu_memory_br_object_manager(br_object_manager *manager)
{
	br_object **objects = get_data_DA(manager->objects);
	for (unsigned int i = 0; i < get_size_DA(manager->objects); i++)
	{
		delete_physic(objects[i]->phy);
		free(objects[i]);
	}
	delete_DA(manager->objects);
	delete_DA(manager->vertices);
	delete_DA(manager->indices);
	manager->objects = 0;
	manager->vertices = 0;
	manager->indices = 0;
}

void set_position_br_object_all(br_object_manager *manager, vec3 v)
{
	glm_translate_make(manager->translation, v);
}

void get_position_br_object_all(br_object_manager *manager, vec3 v)
{
	glm_vec3_copy(manager->translation[3], v);
}

void set_rotation_br_object_all(br_object_manager *manager, float angle, vec3 axis)
{
	glm_rotate_make(manager->rotation, glm_rad(angle), axis);
}