#pragma once
#include "../../third_party/opengl/include/glad/glad.h"
#include "../../third_party/cglm/include/cglm/cglm.h"

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct bodyid bodyid;

  void init_jolt(void);

  void deinit_jolt(void);

  void run_jolt(float cDeltaTime);

  // heightmap must be square because of jolt >:(
  bodyid create_hm_jolt(float *heightmappoints, vec3 offset, vec3 scale, int length);

  void delete_body_jolt(bodyid id);

  void optimize_jolt(void);

#ifdef __cplusplus
}
#endif