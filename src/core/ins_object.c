#include "ins_object.h"

ins_object_manager *create_ins_object_manager(GLfloat *vertices, unsigned int vertex_number, GLuint *indices,
											  unsigned int indice_number)
{
	ins_object_manager *x = malloc(sizeof(ins_object_manager));
	x->VAO = 0;
	x->VBO_geometry = 0;
	x->VBO_model_matrix = 0;
	x->VBO_normal_matrix = 0;
	x->VBO_texture = 0;
	x->EBO = 0;
	x->objects = create_DA_HIGH_MEMORY(sizeof(ins_object *));
	x->vertices = create_DA(sizeof(GLfloat));
	x->indices = create_DA(sizeof(GLuint));
	x->models = create_DA_HIGH_MEMORY(sizeof(mat4));
	x->normals = create_DA_HIGH_MEMORY(sizeof(mat4));
	x->textures = create_DA_HIGH_MEMORY(sizeof(GLfloat));
	x->subdata = 0;
	pushback_many_DA(x->vertices, vertices, vertex_number * 8);
	pushback_many_DA(x->indices, indices, indice_number);
	return x;
}

void delete_ins_object_manager(ins_object_manager *manager)
{
	ins_object **objects = get_data_DA(manager->objects);
	for (unsigned int i = 0; i < get_size_DA(manager->objects); i++)
	{
		delete_physic(objects[i]->phy);
		free(objects[i]);
	}
	delete_DA(manager->objects);
	delete_DA(manager->vertices);
	delete_DA(manager->indices);
	delete_DA(manager->models);
	delete_DA(manager->normals);
	delete_DA(manager->textures);
	glDeleteVertexArrays(1, &(manager->VAO));
	glDeleteBuffers(1, &(manager->VBO_geometry));
	glDeleteBuffers(1, &(manager->VBO_model_matrix));
	glDeleteBuffers(1, &(manager->VBO_normal_matrix));
	glDeleteBuffers(1, &(manager->VBO_texture));
	glDeleteBuffers(1, &(manager->EBO));
	free(manager);
}

ins_object *create_ins_object(ins_object_manager *manager, GLfloat texture_index, unsigned char has_physics,
							  unsigned char priority, float mass, float friction, float bounce)
{
	ins_object *x = malloc(sizeof(ins_object));
	x->manager = manager;

	if (has_physics == 1)
	{
		GLfloat *vertices = get_data_DA(manager->vertices);
		vec3 minaabb, maxaabb;
		for (unsigned int i = 0; i < get_size_DA(manager->vertices) / 8; i++)
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
				if (vertices[i * 8] > maxaabb[0])
				{
					maxaabb[0] = vertices[i * 8];
				}
				if (vertices[i * 8 + 1] > maxaabb[1])
				{
					maxaabb[1] = vertices[i * 8 + 1];
				}
				if (vertices[i * 8 + 2] > maxaabb[2])
				{
					maxaabb[2] = vertices[i * 8 + 2];
				}
				if (vertices[i * 8] < minaabb[0])
				{
					minaabb[0] = vertices[i * 8];
				}
				if (vertices[i * 8 + 1] < minaabb[1])
				{
					minaabb[1] = vertices[i * 8 + 1];
				}
				if (vertices[i * 8 + 2] < minaabb[2])
				{
					minaabb[2] = vertices[i * 8 + 2];
				}
			}
		}
		x->phy = create_physic(minaabb, maxaabb, priority, mass, friction, bounce, x, 1);
	}
	else
	{
		x->phy = 0;
	}

	mat4 ident = GLM_MAT4_IDENTITY_INIT;

	pushback_DA(x->manager->objects, &x);
	pushback_DA(x->manager->models, &ident);
	pushback_DA(x->manager->normals, &ident);
	pushback_DA(x->manager->textures, &(texture_index));

	return x;
}

void delete_ins_object(ins_object *obj)
{
	delete_physic(obj->phy);
	unsigned int index = get_index_DA(obj->manager->objects, &obj);
	remove_DA(obj->manager->objects, index);
	remove_DA(obj->manager->models, index);
	remove_DA(obj->manager->normals, index);
	remove_DA(obj->manager->textures, index);
	free(obj);
}

void update_ins_physic(ins_object *obj)
{
	mat4 *model_mats = get_data_DA(obj->manager->models);
	unsigned int index = get_index_DA(obj->manager->objects, &obj);

	vec4 result;
	glm_mat4_mulv(model_mats[index], (vec4){obj->phy->first_minaabb[0], obj->phy->first_minaabb[1], obj->phy->first_minaabb[2], 1}, result);
	glm_vec3_copy(result, obj->phy->minaabb);
	glm_mat4_mulv(model_mats[index], (vec4){obj->phy->first_maxaabb[0], obj->phy->first_maxaabb[1], obj->phy->first_maxaabb[2], 1}, result);
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

void scale_ins_object(ins_object *obj, vec3 v, unsigned char effect_physic)
{
	mat4 *model_mats = get_data_DA(obj->manager->models);
	mat4 *normal_mats = get_data_DA(obj->manager->normals);
	unsigned int index = get_index_DA(obj->manager->objects, &obj);

	glm_scale(model_mats[index], v);
	glm_mat4_inv(model_mats[index], normal_mats[index]);
	glm_mat4_transpose(normal_mats[index]);
	if (effect_physic == 1 && obj->phy != 0)
	{
		update_ins_physic(obj);
	}
	obj->manager->subdata = 1;
}

void rotate_ins_object(ins_object *obj, float angle, vec3 axis, unsigned char effect_physic)
{
	mat4 *model_mats = get_data_DA(obj->manager->models);
	mat4 *normal_mats = get_data_DA(obj->manager->normals);
	unsigned int index = get_index_DA(obj->manager->objects, &obj);

	glm_rotate(model_mats[index], glm_rad(angle), axis);
	glm_mat4_inv(model_mats[index], normal_mats[index]);
	glm_mat4_transpose(normal_mats[index]);
	if (effect_physic == 1 && obj->phy != 0)
	{
		update_ins_physic(obj);
	}
	obj->manager->subdata = 1;
}

void translate_ins_object(ins_object *obj, vec3 v, unsigned char effect_physic)
{
	mat4 *model_mats = get_data_DA(obj->manager->models);
	mat4 *normal_mats = get_data_DA(obj->manager->normals);
	unsigned int index = get_index_DA(obj->manager->objects, &obj);

	glm_translate(model_mats[index], v);
	glm_mat4_inv(model_mats[index], normal_mats[index]);
	glm_mat4_transpose(normal_mats[index]);
	if (effect_physic == 1 && obj->phy != 0)
	{
		update_ins_physic(obj);
	}
	obj->manager->subdata = 1;
}

void prepare_render_ins_object_manager(ins_object_manager *manager)
{
	// clear
	glDeleteVertexArrays(1, &(manager->VAO));
	glDeleteBuffers(1, &(manager->VBO_geometry));
	glDeleteBuffers(1, &(manager->VBO_model_matrix));
	glDeleteBuffers(1, &(manager->VBO_normal_matrix));
	glDeleteBuffers(1, &(manager->VBO_texture));
	glDeleteBuffers(1, &(manager->EBO));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (get_size_DA(manager->objects) > 0)
	{
		// generate buffers
		glGenVertexArrays(1, &(manager->VAO));
		glGenBuffers(1, &(manager->VBO_geometry));
		glGenBuffers(1, &(manager->VBO_model_matrix));
		glGenBuffers(1, &(manager->VBO_normal_matrix));
		glGenBuffers(1, &(manager->VBO_texture));
		glGenBuffers(1, &(manager->EBO));

		// start assigning data
		glBindVertexArray(manager->VAO);

		// assigning index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, manager->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, get_size_DA(manager->indices) * sizeof(GLuint), get_data_DA(manager->indices), GL_STATIC_DRAW);

		// assigning vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, manager->VBO_geometry);
		glBufferData(GL_ARRAY_BUFFER, get_size_DA(manager->vertices) * sizeof(GLfloat), get_data_DA(manager->vertices), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(5 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		// assigning model matrix
		glBindBuffer(GL_ARRAY_BUFFER, manager->VBO_model_matrix);
		glBufferData(GL_ARRAY_BUFFER, get_size_DA(manager->models) * sizeof(mat4), get_data_DA(manager->models), GL_STATIC_DRAW);
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(3 + i);
			glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(sizeof(float) * i * 4));
			glVertexAttribDivisor(3 + i, 1); // This attribute is instanced
		}

		// assigning normal matrix
		glBindBuffer(GL_ARRAY_BUFFER, manager->VBO_normal_matrix);
		glBufferData(GL_ARRAY_BUFFER, get_size_DA(manager->normals) * sizeof(mat4), get_data_DA(manager->normals), GL_STATIC_DRAW);
		for (unsigned int i = 0; i < 4; ++i)
		{
			glEnableVertexAttribArray(7 + i);
			glVertexAttribPointer(7 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void *)(sizeof(float) * i * 4));
			glVertexAttribDivisor(7 + i, 1); // This attribute is instanced
		}

		// assigning texture index
		glBindBuffer(GL_ARRAY_BUFFER, manager->VBO_texture);
		glBufferData(GL_ARRAY_BUFFER, get_size_DA(manager->textures) * sizeof(GLfloat), get_data_DA(manager->textures), GL_STATIC_DRAW);
		glEnableVertexAttribArray(11);
		glVertexAttribPointer(11, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), 0);
		glVertexAttribDivisor(11, 1); // This attribute is instanced

		// end assigning data
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void use_ins_object_manager(ins_object_manager *manager)
{
	if (get_size_DA(manager->objects) > 0)
	{
		if (manager->subdata == 1)
		{
			glBindBuffer(GL_ARRAY_BUFFER, manager->VBO_model_matrix);
			glBufferSubData(GL_ARRAY_BUFFER, 0, get_size_DA(manager->models) * sizeof(mat4), get_data_DA(manager->models));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, manager->VBO_normal_matrix);
			glBufferSubData(GL_ARRAY_BUFFER, 0, get_size_DA(manager->normals) * sizeof(mat4), get_data_DA(manager->normals));
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			manager->subdata = 0;
		}
		glBindVertexArray(manager->VAO);
		glDrawElementsInstanced(GL_TRIANGLES, get_size_DA(manager->indices), GL_UNSIGNED_INT, 0, get_size_DA(manager->objects));
		glBindVertexArray(0);
	}
}