#pragma once
#ifdef _MSC_VER
#pragma warning(disable : 4324) // stupid struct padding warning
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