#include "load_object.h"
#include "../../third_party/assimp/include/assimp/cimport.h"
#include "../../third_party/assimp/include/assimp/scene.h"
#include "../../third_party/assimp/include/assimp/postprocess.h"

void load_object_br(br_object_manager *obj_manager, br_texture_manager *text_manager,
					const char *path, float texture_start_index, unsigned char has_physics,
					unsigned char priority, float mass, float friction, float bounce)
{
	text_manager;
	const struct aiScene *scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
														 aiProcess_TransformUVCoords | aiProcess_PreTransformVertices);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		unsigned int vertex_number = scene->mMeshes[i]->mNumVertices;
		GLfloat *vertices = (GLfloat *)malloc(sizeof(GLfloat) * vertex_number * 9);
		for (unsigned int k = 0; k < vertex_number; k++)
		{
			vertices[k * 9] = scene->mMeshes[k]->mVertices[k].x;
			vertices[k * 9 + 1] = scene->mMeshes[k]->mVertices[k].y;
			vertices[k * 9 + 2] = scene->mMeshes[k]->mVertices[k].z;

			vertices[k * 9 + 3] = scene->mMeshes[k]->mTextureCoords[0][k].x;
			vertices[k * 9 + 4] = scene->mMeshes[k]->mTextureCoords[0][k].y;

			vertices[k * 9 + 5] = scene->mMeshes[k]->mNormals[k].x;
			vertices[k * 9 + 6] = scene->mMeshes[k]->mNormals[k].y;
			vertices[k * 9 + 7] = scene->mMeshes[k]->mNormals[k].z;

			vertices[k * 9 + 8] = 0;
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
		create_br_object(obj_manager, vertices, vertex_number, indices, indice_number,
						 texture_start_index, has_physics, priority, mass, friction, bounce);
		free(vertices);
		free(indices);
	}
	aiReleaseImport(scene);
}