#include "load_object.h"
#include "../../third_party/assimp/include/assimp/cimport.h"
#include "../../third_party/assimp/include/assimp/postprocess.h"

struct aiScene *load_model(const char *path, unsigned char flip_order)
{
	if (flip_order == 0)
	{
		return (struct aiScene *)aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
														aiProcess_TransformUVCoords | aiProcess_PreTransformVertices);
	}
	else
	{
		return (struct aiScene *)aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
														aiProcess_TransformUVCoords | aiProcess_PreTransformVertices | aiProcess_FlipWindingOrder);
	}
}

void free_model(struct aiScene *scene)
{
	aiReleaseImport(scene);
}

br_scene load_object_br(br_object_manager *obj_manager, br_texture_manager *text_manager,
						struct aiScene *scene, float texture_start_index, unsigned char has_physics,
						unsigned char priority, float mass, float friction, float bounce)
{
	br_scene res;
	res.mesh_count = scene->mNumMeshes;
	res.texture_count = scene->mNumMaterials;
	res.textures = (br_texture **)malloc(sizeof(br_texture *) * res.texture_count);
	res.meshes = (br_object **)malloc(sizeof(br_object *) * res.mesh_count);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		struct aiString texturePath = {0};
		struct aiColor4D diffuseColor = {0};
		if (aiGetMaterialTexture(scene->mMaterials[i], aiTextureType_DIFFUSE, 0, &texturePath, 0, 0, 0, 0, 0, 0) == aiReturn_SUCCESS)
		{
			unsigned char embedded = 0;
			for (unsigned int k = 0; k < scene->mNumTextures; k++)
			{
				if (strcmp(scene->mTextures[k]->mFilename.data, texturePath.data) == 0)
				{
					embedded = 1;
					int width, height;
					unsigned char *data;
					width = scene->mTextures[k]->mWidth;
					height = scene->mTextures[k]->mHeight;
					if (height == 0)
					{
						break;
					}
					data = malloc(sizeof(unsigned char) * width * height * 4);
					for (int j = 0; j < width * height; j++)
					{
						data[j * 4] = scene->mTextures[k]->pcData[j].r;
						data[j * 4 + 1] = scene->mTextures[k]->pcData[j].g;
						data[j * 4 + 2] = scene->mTextures[k]->pcData[j].b;
						data[j * 4 + 3] = scene->mTextures[k]->pcData[j].a;
					}
					res.textures[i] = create_br_texture_memory(text_manager, data, width, height, GL_TEXTURE_2D, GL_NEAREST,
															   GL_NEAREST, (int)texture_start_index + (int)i);
					free(data);
					break;
				}
			}
			if (embedded == 0)
			{
				char ext_path[1024] = {0};
				sprintf(ext_path, "./models/%s", texturePath.data);
				res.textures[i] = create_br_texture(text_manager, ext_path, GL_TEXTURE_2D, GL_NEAREST, GL_NEAREST, (int)texture_start_index + (int)i);
			}
		}
		else if (aiGetMaterialColor(scene->mMaterials[i], AI_MATKEY_COLOR_DIFFUSE, &diffuseColor) == AI_SUCCESS)
		{
			unsigned char data[4];
			data[0] = (unsigned char)__max(0, __min(255, (int)floorf(diffuseColor.r * 256.0f)));
			data[1] = (unsigned char)__max(0, __min(255, (int)floorf(diffuseColor.g * 256.0f)));
			data[2] = (unsigned char)__max(0, __min(255, (int)floorf(diffuseColor.b * 256.0f)));
			data[3] = (unsigned char)__max(0, __min(255, (int)floorf(diffuseColor.a * 256.0f)));
			res.textures[i] = create_br_texture_memory(text_manager, data, 1, 1, GL_TEXTURE_2D, GL_NEAREST,
													   GL_NEAREST, (int)texture_start_index + (int)i);
		}
	}
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		unsigned int vertex_number = scene->mMeshes[i]->mNumVertices;
		GLfloat *vertices = (GLfloat *)malloc(sizeof(GLfloat) * vertex_number * 9);
		for (unsigned int k = 0; k < vertex_number; k++)
		{
			vertices[k * 9] = scene->mMeshes[i]->mVertices[k].x;
			vertices[k * 9 + 1] = scene->mMeshes[i]->mVertices[k].y;
			vertices[k * 9 + 2] = scene->mMeshes[i]->mVertices[k].z;

			vertices[k * 9 + 3] = scene->mMeshes[i]->mTextureCoords[0][k].x;
			vertices[k * 9 + 4] = scene->mMeshes[i]->mTextureCoords[0][k].y;

			vertices[k * 9 + 5] = scene->mMeshes[i]->mNormals[k].x;
			vertices[k * 9 + 6] = scene->mMeshes[i]->mNormals[k].y;
			vertices[k * 9 + 7] = scene->mMeshes[i]->mNormals[k].z;

			vertices[k * 9 + 8] = scene->mMeshes[i]->mMaterialIndex + texture_start_index;
		}
		unsigned int indice_number = 0;
		for (unsigned int k = 0; k < scene->mMeshes[i]->mNumFaces; k++)
		{
			indice_number += scene->mMeshes[i]->mFaces[k].mNumIndices;
		}
		GLuint *indices = (GLuint *)malloc(sizeof(GLuint) * indice_number);
		unsigned int index = 0;
		for (unsigned int k = 0; k < scene->mMeshes[i]->mNumFaces; k++)
		{
			for (unsigned int j = 0; j < scene->mMeshes[i]->mFaces[k].mNumIndices; j++)
			{
				indices[index] = scene->mMeshes[i]->mFaces[k].mIndices[j];
				index++;
			}
		}
		res.meshes[i] = create_br_object(obj_manager, vertices, vertex_number, indices, indice_number,
										 scene->mMeshes[i]->mMaterialIndex + texture_start_index - 1,
										 has_physics, priority, mass, friction, bounce);
		free(vertices);
		free(indices);
	}
	return res;
}