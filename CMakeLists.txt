cmake_minimum_required(VERSION 3.10)

project(voxel_engine C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'")

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}$<0:>)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)

set(CGLM_SHARED OFF CACHE BOOL "" FORCE)
set(CGLM_STATIC ON CACHE BOOL "" FORCE)
set(CGLM_USE_C99 ON CACHE BOOL "" FORCE)
set(CGLM_USE_TEST OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_NO_EXPORT ON CACHE BOOL "" FORCE)
set(USE_STATIC_CRT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)

set(SOLOUD_STATIC ON CACHE BOOL "" FORCE)
set(SOLOUD_C_API ON CACHE BOOL "" FORCE)
set(SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE)

set(OVERRIDE_CXX_FLAGS ON CACHE BOOL "" FORCE)
set(OVERRIDE_C_FLAGS ON CACHE BOOL "" FORCE)

if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")

	set(SOLOUD_BACKEND_XAUDIO2 ON CACHE BOOL "" FORCE)

	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
	set(CMAKE_GENERATOR_PLATFORM x64)
  set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /Oi /Ot /Oy /GT /GL /MT /GS- /W4 /wd4324 /MP /Gy")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Oi /Ot /Oy /GT /GL /MT /GS- /W4 /wd4324 /MP /Gy")
  set(CMAKE_EXE_LINKER_FLAGS "/LTCG /machine:x64 /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup
	/NODEFAULTLIB:MSVCRT /NODEFAULTLIB:LIBCMT")
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	add_subdirectory(third_party/glfw)
	add_subdirectory(third_party/cglm)
	add_subdirectory(third_party/assimp)
	add_subdirectory(third_party/freetype)
	add_subdirectory(third_party/jolt/Build)
	add_subdirectory(third_party/soloud/contrib)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/cglm/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/opengl/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/third_party/assimp/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/jolt)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/soloud/include)

	FILE(GLOB_RECURSE MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
	add_executable(
	voxel_engine
	${MY_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/opengl/src/glad.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/jolt_physics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/threading.cpp
	)

	find_package(OpenGL REQUIRED)
	target_link_libraries(voxel_engine cglm glfw assimp freetype Jolt msvcrt OpenGL::GL soloud)
	target_compile_options(voxel_engine PUBLIC /MT)

	set_target_properties(voxel_engine PROPERTIES WIN32_EXECUTABLE TRUE)

elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU")

	set(SOLOUD_BACKEND_ALSA ON CACHE BOOL "" FORCE)

	set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -fno-stack-protector -D_FORTIFY_SOURCE=0")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -fno-stack-protector -D_FORTIFY_SOURCE=0")
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -s")
	endif()
	set(CMAKE_BUILD_PARALLEL_LEVEL "$(nproc)")

	set(CMAKE_C_FLAGS_DEBUG "-Wall -ggdb3")
	set(CMAKE_CXX_FLAGS_DEBUG "-Wall -ggdb3")

	add_subdirectory(third_party/glfw)
	add_subdirectory(third_party/cglm)
	add_subdirectory(third_party/assimp)
	add_subdirectory(third_party/freetype)
	add_subdirectory(third_party/jolt/Build)
	add_subdirectory(third_party/soloud/contrib)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/cglm/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/opengl/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/build/third_party/assimp/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/freetype/include)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/jolt)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/soloud/include)

	FILE(GLOB_RECURSE MY_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
	add_executable(
	voxel_engine
	${MY_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/third_party/opengl/src/glad.c
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/jolt_physics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/core/threading.cpp
	)

	set(OpenGL_GL_PREFERENCE GLVND)
	find_package(OpenGL REQUIRED)
	target_link_libraries(voxel_engine cglm glfw assimp freetype Jolt OpenGL::GL soloud)

else()

  message(FATAL_ERROR "You can compile this with MSVC on x64 windows computer or GNU on x64 ubuntu computer.")
	
endif()
