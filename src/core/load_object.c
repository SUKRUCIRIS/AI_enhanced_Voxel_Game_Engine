#include "load_object.h"
#include "../../third_party/assimp/include/assimp/cimport.h"
#include "../../third_party/assimp/include/assimp/scene.h"
#include "../../third_party/assimp/include/assimp/postprocess.h"

void load_object_br(br_object_manager *obj_manager, br_texture_manager *text_manager,
					const char *path, int scene_index, int texture_start_index, unsigned char has_physics,
					unsigned char priority, float mass, float friction, float bounce)
{
	const struct aiScene *scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
														 aiProcess_TransformUVCoords | aiProcess_PreTransformVertices);

	aiReleaseImport(scene);
}