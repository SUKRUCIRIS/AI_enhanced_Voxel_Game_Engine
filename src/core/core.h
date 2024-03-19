#pragma once
#ifdef _MSC_VER
#pragma warning(disable : 4324) // stupid struct padding warning
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#include "../../third_party/opengl/include/glad/glad.h"
#include "../../third_party/glfw/include/GLFW/glfw3.h"
#include "../../third_party/stb/stb_image.h"
#include "../../third_party/cglm/include/cglm/cglm.h"
#include "camera.h"
#include "dynamic.h"
#include "lighting.h"
#include "object.h"
#include "physics.h"
#include "shaders.h"
#include "texture.h"
#include "window.h"
#include "br_object.h"
#include "br_texture.h"
#include "snoise.h"
#include "ins_object.h"
#include "load_object.h"
#include "timing.h"
#include "animation.h"
#include "random.h"
#include "font.h"
#include "skybox.h"
#include "jolt_physics.h"
#include "player.h"
#include "world_batch.h"
#include "world_instanced.h"
#include "chunk.h"
#include "threading.h"
#include "water.h"
#ifdef __cplusplus
}
#endif